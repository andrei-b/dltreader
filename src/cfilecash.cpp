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

#include "cfilecash.h"
extern "C" {
#include "dltbase/dlt_common.h"
}
#include <cstring>
#include <stdio.h>

CFileCash::CFileCash(const std::string &fileName) : mFileName(fileName)
{
    rf.open(fileName);
    buffers[1].buf = new u_char[BufferSize];
    if (rf.is_open())
        runOverMessages();
}

CFileCash::~CFileCash()
{
    setEof();
    wake();
    rf.close();
}

bool CFileCash::getMessage(int32_t num, DltMessagePtr &msg)
{
    while (true) {
        if (buffers[minConsumerBuffer].state == FileCashBufferState::fcbsReadyForConsumer) {
            if (num < buffers[minConsumerBuffer].firstMessage)
                return false; //trying to read before cash
            if (num < buffers[minConsumerBuffer].firstMessage+buffers[minConsumerBuffer].messageCount) {
                //hit!
                return true;
            } else {
                // not in this buffer, releasing
                passBufferToProducer(minConsumerBuffer);
                minConsumerBuffer = 1 - minConsumerBuffer;
            }
        }
        if (eof())
            return false;
        waitForConsumerReady();
    }
}

bool CFileCash::fileOpen()
{
    return rf.is_open();
}


void CFileCash::passBufferToConsumer(uint num)
{
    buffers[num].state = FileCashBufferState::fcbsReadyForConsumer;
    wake();
}

void CFileCash::passBufferToProducer(uint num)
{
    buffers[num].state = FileCashBufferState::fcbsReadyForProducer;
    wake();
}

void CFileCash::waitForProducerReady()
{
    std::unique_lock<std::mutex> lk(mutex);
    cv.wait(lk, [this](){if (buffers[nextBufferToProduce].state == FileCashBufferState::fcbsReadyForProducer) return true; return false;});
}

void CFileCash::waitForConsumerReady()
{
    std::unique_lock<std::mutex> lk(mutex);
    cv.wait(lk, [this](){return buffers[0].state == FileCashBufferState::fcbsReadyForConsumer | buffers[1].state == FileCashBufferState::fcbsReadyForConsumer | mEof;});
}

void CFileCash::wake()
{
    cv.notify_one();
}

void CFileCash::setEof()
{
    mEof = true;
    wake();
}

void CFileCash::fillNextBuffer()
{
    if (eof()||!fileOpen())
        return;
    waitForProducerReady();
    auto offset = mIndex[nextMessageIndex];
   // for (auto i = nextMessageIndex; i < )

}

int32_t sstrstr1(char *haystack, char *needle, size_t begin, size_t length)
{
    size_t needle_length = strlen(needle);
    size_t i;
    for (i = begin; i < length; i++) {
        if (i + needle_length > length) {
            return -1;
        }
        if (strncmp(&haystack[i], needle, needle_length) == 0) {
            return &haystack[i] - &haystack[0];
        }
    }
    return -1;
}

void CFileCash::runOverMessages()
{
    const auto TotalHeaderSize = sizeof(DltStorageHeader) + sizeof(DltStandardHeader);
    rf.seekg (0, rf.end);
    int64_t fileSize = rf.tellg();
    rf.seekg (0, rf.beg);
    uint64_t currentMsgPosGlobal = 0;
    uint64_t nextMsgPosGlobal = 0;
    uint64_t bytesReadTotal = 0;
    uint32_t bytesRead = 0;
    int32_t currentMsgPosLocal = 0;
    uint32_t nextMsgPosLocal = 0;
    uint64_t localMsgOffset = 0;
    while(true)
    {
        /* read buffer from file */
        while (nextMsgPosGlobal >= bytesReadTotal) {
            rf.read((char*)buffers[1].buf, BufferSize);
            bytesRead = rf.gcount();
            if (bytesRead == 0)
                break; // EOF
            localMsgOffset = bytesReadTotal;
            bytesReadTotal +=  bytesRead;
            if (buffers[1].buf[bytesRead-1] == 'D' || buffers[1].buf[bytesRead-1] == 'L' || buffers[1].buf[bytesRead-1] == 'T') {
                bytesReadTotal -= 3;
                bytesRead -= 3;
                rf.seekg(bytesReadTotal);

            }
        }
        if (bytesRead == 0)
                break; // EOF

        nextMsgPosLocal = nextMsgPosGlobal-localMsgOffset;
        currentMsgPosLocal = sstrstr1((char*)buffers[1].buf, (char*)"DLT\1", nextMsgPosLocal, bytesRead);
        if (currentMsgPosLocal < 0) {
            nextMsgPosGlobal = bytesReadTotal;
            continue;
        }
        DltStandardHeader * hdr = (DltStandardHeader *) &buffers[1].buf[currentMsgPosLocal+sizeof(DltStorageHeader)];
        uint16_t msgLen = DLT_BETOH_16(hdr->len) + 16;
        //printf("i:%i\n", xLen);
        currentMsgPosGlobal = currentMsgPosLocal + localMsgOffset;
        nextMsgPosGlobal = currentMsgPosGlobal + msgLen;
        nextMsgPosLocal = currentMsgPosLocal + msgLen;
        if (nextMsgPosLocal + 4 > bytesRead) {
            rf.seekg(currentMsgPosGlobal);
            bytesReadTotal = currentMsgPosGlobal;
            nextMsgPosGlobal = currentMsgPosGlobal;
        } else {
            mRecordsCount++;
            bool good = true;
            auto hdr0 = (DltStorageHeader *) &buffers[1].buf[nextMsgPosLocal];
            if (sstrstr1((char*)hdr0->pattern, "DLT\1", 0, 4) != 0) {
                printf("Error\n");
                good = false;
            }
            onNewRecord(this, mRecordsCount, currentMsgPosGlobal, (DltStorageHeader *) &buffers[1].buf[currentMsgPosLocal], msgLen);
            if (nextMsgPosLocal < bytesRead && bytesRead - nextMsgPosLocal <= TotalHeaderSize) {
                    bytesReadTotal -= TotalHeaderSize;
                    rf.seekg(bytesReadTotal);
            }
        }
    }
    {
        auto hdr0 = (DltStorageHeader *) &buffers[1].buf[nextMsgPosLocal];
        if (sstrstr1((char*)hdr0->pattern, "DLT\1", 0, 4) != 0) {
            DltStandardHeader * hdr = (DltStandardHeader *) &buffers[1].buf[nextMsgPosLocal+sizeof(DltStorageHeader)];
            uint16_t msgLen = DLT_BETOH_16(hdr->len) + 16;
            mRecordsCount++;
            onNewRecord(this, mRecordsCount, currentMsgPosGlobal, (DltStorageHeader *) &buffers[1].buf[nextMsgPosLocal], msgLen);
        }
    }
    printf("ifirst: %i\n", index[0]);
    printf("ilast: %i\n", index[index.size()-1]); // 118267231 118267312
}

bool CFileCash::addRecordToIndex(CFileCash *instance, uint32_t number, uint64_t globalPos, const DltStorageHeader *ptr, uint16_t length)
{
    (void)ptr;
    (void)length;
    instance->index.push_back(globalPos);
    return true;
}

uint32_t CFileCash::recordsCount()
{
    return mRecordsCount;
}
