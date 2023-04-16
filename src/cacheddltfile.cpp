/*
    DLTReader Diagnostic Log and Trace reading library
    Copyright (C) 2023 Andrei Borovsky <andrey.borovsky@gmail.com>
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "cacheddltfile.h"
#include <algorithm>
#include <thread>
#include <stdio.h>

namespace DLTFile {

CachedDLTFile::CachedDLTFile(const std::string &fileName) : DLTFileParser(fileName)
{
    for (auto &b : buffers)
        b.buf.reserve(BufferSize);
}

CachedDLTFile::~CachedDLTFile()
{
    setEof();
    waitForProducerFinished();
    closeFile();
}

bool CachedDLTFile::eof() const
{
    return mEof;
}

void CachedDLTFile::runCache()
{
    uint oldCounter = runCounter;
    std::thread producer(&CachedDLTFile::produce, this);
    {
        std::unique_lock<std::mutex> lk(mutex);
        cv.wait(lk, [this, oldCounter](){return runCounter != oldCounter;});
    }
    producer.detach();
}

bool CachedDLTFile::init()
{
    if (!initialized) {
        initialized = DLTFileParser::init();
        if (initialized)
            runCache();
    }
    return initialized;
}

void CachedDLTFile::reset()
{
    setEof();
    waitForProducerFinished();
    resetFile();
    nextBufferToConsume = 0;
    currentBufferToConsume = 0;
    fileOffset = 0;
    mEof = false;
    for (auto & b : buffers) {
        b.bytesCount = 0;
        b.offsetInfile = 0;
        b.state = bsReadyForProducer;
    }
    initialized = false;
    init();
}

bool CachedDLTFile::resetFile()
{
    mFile.close();
    mFile.open(fileName());
    if (mFile.is_open()) {
        mFile.seekg (0, mFile.end);
        mFileSize = mFile.tellg();
        mFile.seekg (0, mFile.beg);
        return true;
    }
    return false;
}

uint64_t CachedDLTFile::readFile(BufPtr &buf)
{
    if (currentBufferToConsume != nextBufferToConsume)
        passBufferToProducer(buffers[currentBufferToConsume]);
    currentBufferToConsume = nextBufferToConsume;
    auto & consumerBuffer = buffers[currentBufferToConsume];
    waitForConsumerReady(consumerBuffer);
    if (consumerBuffer.bytesCount == 0)
        return 0;
    if (consumerBuffer.offsetInfile <= fileOffset && consumerBuffer.offsetInfile + consumerBuffer.bytesCount > fileOffset) {
        uint64_t offsetInBuffer = fileOffset - consumerBuffer.offsetInfile;
        buf = &consumerBuffer.buf.data()[offsetInBuffer];
        uint64_t bytesRead = consumerBuffer.bytesCount - offsetInBuffer;
        fileOffset += bytesRead;
        nextBufferToConsume = (nextBufferToConsume + 1) % BufferCount;
        return  bytesRead;
    } else {
        throw std::exception();
    }
    return 0;
}

void CachedDLTFile::seek(uint64_t pos)
{
    fileOffset = pos;
}

void CachedDLTFile::closeFile()
{
    mFile.close();
}

void CachedDLTFile::passBufferToConsumer(CachedBuffer &buf)
{
    buf.state = BufferState::bsReadyForConsumer;
    wake();
}

void CachedDLTFile::passBufferToProducer(CachedBuffer &buf)
{
    buf.state = BufferState::bsReadyForProducer;
    wake();
}

void CachedDLTFile::waitForProducerReady()
{
    std::unique_lock<std::mutex> lk(mutex);
    cv.wait(lk, [this](){return buffers[nextBufferToProduce].state == BufferState::bsReadyForProducer | mEof;});
}

void CachedDLTFile::waitForConsumerReady(CachedBuffer &buf)
{
    std::unique_lock<std::mutex> lk(mutex);
    cv.wait(lk, [&buf, this](){return buf.state == BufferState::bsReadyForConsumer | mEof;});
}

void CachedDLTFile::waitForProducerFinished()
{
    std::unique_lock<std::mutex> lk(mutex);
    cv.wait(lk, [this](){return producerFinished;});
}

void CachedDLTFile::waitForProducerStarted()
{
    std::unique_lock<std::mutex> lk(mutex);
    cv.wait(lk, [this](){return !producerFinished||mEof;});
}

void CachedDLTFile::wake()
{
    cv.notify_all();
}

void CachedDLTFile::setEof()
{
    mEof = true;
    wake();
}

void CachedDLTFile::produce()
{
    producerFinished = false;
    runCounter++;
    int lastProducedBuffer = 0;
    nextBufferToProduce = 1;
    uint64_t totalBytesReadByProducer = 0;
    char* ptr = &buffers[0].buf.data()[0];
    mFile.read(ptr, BufferSize);
    uint64_t bytesRead = mFile.gcount();
    if (bytesRead == 0) {
        buffers[nextBufferToProduce].bytesCount = 0;
        buffers[nextBufferToProduce].state = BufferState::bsReadyForConsumer;
        for(auto &b : buffers)
            if (b.state == BufferState::bsReadyForProducer)
                b.bytesCount = 0;
        setEof();
    }
    buffers[0].bytesCount = bytesRead;
    buffers[0].offsetInfile = 0;
    totalBytesReadByProducer = bytesRead;
    passBufferToConsumer(buffers[0]);
    while (!mEof) {
        waitForProducerReady();
        auto & lpb = buffers[lastProducedBuffer];
        auto & nbtp = buffers[nextBufferToProduce];
        uint64_t bytesToCopy = std::min<uint>(lpb.bytesCount, 0xffff);
        std::copy(&lpb.buf.data()[lpb.bytesCount]-bytesToCopy, &lpb.buf.data()[lpb.bytesCount], nbtp.buf.data());
        //printf("bytesCount: %i\n", buffers[lastProducedBuffer].bytesCount);
        //printf("bytesToCopy: %i\n", bytesToCopy);
        char* ptr = &nbtp.buf.data()[bytesToCopy];
        uint pos = mFile.tellg();
        if (bytesToCopy > pos)
            printf("FAIL");
        nbtp.offsetInfile = totalBytesReadByProducer - bytesToCopy;
        mFile.read(ptr, BufferSize-bytesToCopy);
        bytesRead = mFile.gcount();
        if (bytesRead == 0) {
            nbtp.bytesCount = 0;
            nbtp.state = BufferState::bsReadyForConsumer;
            for(auto &b : buffers)
                if (b.state == BufferState::bsReadyForProducer)
                    b.bytesCount = 0;
            setEof();
        } else {
            nbtp.bytesCount = bytesToCopy + bytesRead;
            totalBytesReadByProducer += bytesRead;
            passBufferToConsumer(nbtp);
            lastProducedBuffer = nextBufferToProduce;
            nextBufferToProduce = (nextBufferToProduce + 1) % BufferCount;
        }
    }
    producerFinished = true;
    wake();
}

}
