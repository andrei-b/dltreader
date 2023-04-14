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

#ifndef CFORWARDCASH_H
#define CFORWARDCASH_H

#include "dlt_common.h"
#include <condition_variable>
#include <mutex>
#include <string>
#include<iostream>
#include<fstream>
#include <vector>

class CFileCash
{
    static const uint BufferCount = 2; // do not change
    static const uint BufferSize = 2048*1024; // this should be greater than the maximum size of the dlt message (64k).
    enum FileCashBufferState {
        fcbsReadyForProducer = 0,
        fcbsReadyForConsumer = 1
    };
    struct FileCashBuffer {
        FileCashBufferState state = fcbsReadyForProducer;
        int32_t firstMessage = -1;
        int32_t firstMessageIndex = 0;
        uint messageCount = 0;
        u_char * buf;
    };
    typedef bool (*NewRecordCallback)(CFileCash * instance, uint32_t number, uint64_t globalPos, const DltStorageHeader * ptr, uint16_t length);
public:
    using DltMessagePtr = DltMessage *;
    explicit CFileCash(const std::string & fileName);
    ~CFileCash();
    bool getMessage(int32_t num, DltMessagePtr & msg);
    bool getNextMessage(DltMessagePtr & msg) {
        return getMessage(nextMessageToConsume++, msg);
    }
    bool eof() {
        return mEof;
    }
    void resetLock();
    bool fileOpen();
    uint32_t recordsCount();
 private:
    std::string mFileName;
    std::ifstream rf;
    long * mIndex = nullptr; // set in the open to point to index
    DltFile fileDescriptor;
    int32_t nextMessageToConsume = 0;
    int32_t nextMessageToProduce = 0;
    int32_t nextMessageIndex = 0; // nextMessageToProduce's index in the index
    uint nextBufferToProduce = 0; // where prod will write
    uint minConsumerBuffer = 0; // first for consumer to check
    uint32_t mRecordsCount = 0;
    FileCashBuffer buffers[BufferCount];
    std::mutex mutex;
    std::condition_variable cv;
    bool mEof = false;
    NewRecordCallback onNewRecord = addRecordToIndex;
    std::vector<uint64_t> index;
    std::vector<uint32_t> badRecords;
    void passBufferToConsumer(uint num);
    void passBufferToProducer(uint num);
    void waitForProducerReady(); // wait till the state is ready for prod
    void waitForConsumerReady();
    void wake();
    void setEof();
    void fillNextBuffer();
    void runOverMessages();
     static bool addRecordToIndex(CFileCash * instance, uint32_t number, uint64_t globalPos, const DltStorageHeader * ptr, uint16_t length);
public:
};

#endif // CFORWARDCASH_H
