/*
    DLTReader Diagnostic Log and Trace reading library
    Copyright (C) 2023 Andrei Borovsky <andrei.borovsky@gmail.com>
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef CACHEDDLTFILE_H
#define CACHEDDLTFILE_H

#include "dltfileparser.h"
#include <vector>
#include <mutex>
#include <condition_variable>
#include <string>
#include<iostream>
#include<fstream>

namespace DLTFile {

class CachedDLTFile : public DLTFileParser
{
    static const uint BufferCount = 2; // do not change
    static const uint64_t BufferSize = 4096*1024; // this should be greater than the maximum size of the dlt message (64k).
    enum BufferState {
        bsReadyForProducer = 0,
        bsReadyForConsumer = 1
    };
    struct CachedBuffer {
        BufferState state = bsReadyForProducer;
        uint bytesCount = 0;
        uint offsetInfile = 0; //offset of the beginning of the buffer relative to the file
        std::vector<char> buf;
    };
public:
    explicit CachedDLTFile(const std::string & fileName);
    virtual ~CachedDLTFile();
    bool eof() const;
    bool init() override;
    void reset() override;
protected:
    bool resetFile() override;
    virtual uint64_t readFile(BufPtr &buf) override;
    void seek(uint64_t pos) override;
    void closeFile() override;
private:
    std::ifstream mFile;
    uint64_t mFileSize;
    CachedBuffer buffers[BufferCount];
    uint fileOffset = 0; // byte from which nex read will start
    std::mutex mutex;
    std::condition_variable cv;
    bool mEof = false;
    int nextBufferToConsume = 0;
    int currentBufferToConsume = 0;
    int nextBufferToProduce = 0;
    bool producerFinished = true;
    uint runCounter = 0;
    bool initialized = false;
    void passBufferToConsumer(CachedBuffer & buf);
    void passBufferToProducer(CachedBuffer &buf);
    void waitForProducerReady(); // wait till the state is ready for prod
    void waitForConsumerReady(CachedBuffer &buf);
    void waitForProducerFinished();
    void waitForProducerStarted();
    void wake();
    void setEof();
    void produce();
    void runCache();
};

}

#endif // CACHEDDLTFILE_H
