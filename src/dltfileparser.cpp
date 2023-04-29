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

#include "dltrecordparser.h"
#include "dltfileparser.h"
#include "string.h"
extern "C" {
#include "dlt_common.h"
}

namespace DLTReader {

const int32_t TotalHeaderSize = sizeof(DltStorageHeader) + sizeof(DltStandardHeader);

DLTFileParser::DLTFileParser(const std::string &fileName) : mFileName(fileName)
{

}

DLTFileParser::DLTFileParser(DLTFileParser &&other)
{
    buffer = other.buffer;
    other.buffer = nullptr;
    mFileName = other.mFileName;
    endOfRecords = other.endOfRecords;
    mRecordsCount = other.mRecordsCount;
    currentMsgPosGlobal = other.currentMsgPosGlobal;
    nextMsgPosGlobal = other.nextMsgPosGlobal;
    bytesReadTotal = other.bytesReadTotal;
    bytesRead = other.bytesRead;
    currentMsgPosLocal = other.currentMsgPosLocal;
    nextMsgPosLocal = other.nextMsgPosLocal;
    localMsgOffset = other.localMsgOffset;
    other.endOfRecords = true;
    other.mRecordsCount = 0;
    other.currentMsgPosGlobal = 0;
    other.nextMsgPosGlobal = 0;
    other.bytesReadTotal = 0;
    other.bytesRead = 0;
    other.currentMsgPosLocal = 0;
    other.nextMsgPosLocal = 0;
    other.localMsgOffset = 0;
}

DLTFileParser::~DLTFileParser()
{
}

const std::string &DLTFileParser::fileName()
{
    return mFileName;
}

bool DLTFileParser::init()
{
    endOfRecords = false;
    mRecordsCount = 0;
    currentMsgPosGlobal = 0;
    nextMsgPosGlobal = 0;
    bytesReadTotal = 0;
    bytesRead = 0;
    currentMsgPosLocal = 0;
    nextMsgPosLocal = 0;
    localMsgOffset = 0;
    return resetFile();
}

void DLTFileParser::reset()
{
    init();
}

int32_t sstrstr(char *haystack, char *needle, size_t begin, size_t length)
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

ParserState DLTFileParser::parse(DLTFileRecord &record)
{
    if (endOfRecords)
        return ParserState::EndOfFile;
    /* read buffer from file */
    while (nextMsgPosGlobal >= bytesReadTotal) {
        bytesRead = readFile(buffer);
        if (bytesRead == 0) {
            endOfRecords = true;
            return ParserState::EndOfFile;
        }
        localMsgOffset = bytesReadTotal;
        bytesReadTotal +=  bytesRead;
        if (buffer[bytesRead-1] == 'D' || buffer[bytesRead-1] == 'L' || buffer[bytesRead-1] == 'T') {
            bytesReadTotal -= 3;
            bytesRead -= 3;
            seek(bytesReadTotal);
        }
    }
    if (bytesRead == 0) {
        endOfRecords = true;
        return processLastRecord(record);
    }
    nextMsgPosLocal = nextMsgPosGlobal-localMsgOffset;
    currentMsgPosLocal = sstrstr((char*)buffer, (char*)"DLT\1", nextMsgPosLocal, bytesRead);
    if (currentMsgPosLocal < 0) {
        nextMsgPosGlobal = bytesReadTotal;
        return ParserState::NeedMoreData;
    }
    DltStandardHeader * hdr = (DltStandardHeader *) &buffer[currentMsgPosLocal+sizeof(DltStorageHeader)];
    length_t msgLen = DLT_BETOH_16(hdr->len) + 16;
    //printf("i:%i\n", xLen);
    currentMsgPosGlobal = currentMsgPosLocal + localMsgOffset;
    nextMsgPosGlobal = currentMsgPosGlobal + msgLen;
    nextMsgPosLocal = currentMsgPosLocal + msgLen;
    if (nextMsgPosLocal + 4 > bytesRead) {
        seek(currentMsgPosGlobal);
        bytesReadTotal = currentMsgPosGlobal;
        nextMsgPosGlobal = currentMsgPosGlobal;
        return ParserState::NeedMoreData;
    } else {
        mRecordsCount++;
        bool good = true;
        auto hdr0 = (DltStorageHeader *) &buffer[nextMsgPosLocal];
        if (sstrstr((char*)hdr0->pattern, (char*)"DLT\1", 0, 4) != 0) {
            printf("Error\n");
            good = false;
        }
        DltStandardHeader * hdr = (DltStandardHeader *) &buffer[nextMsgPosLocal+sizeof(DltStorageHeader)];
        length_t msgLen = DLT_BETOH_16(hdr->len) + 16;
        record = {mRecordsCount, currentMsgPosGlobal, true, msgLen, (char*)hdr0, false};
        if (nextMsgPosLocal < bytesRead && bytesRead - nextMsgPosLocal <= TotalHeaderSize) {
                bytesReadTotal -= TotalHeaderSize;
                seek(bytesReadTotal);
        }
        return ParserState::HaveRecord;
    }
    return ParserState::Error;
}

ParserState DLTFileParser::parseBatch(RecordCollection &records)
{
    DLTFileRecord r;
    auto state = parse(r);
    while (state == ParserState::HaveRecord && !nextCallWillRead()) {
        records.push_back(r);
        state = parse(r);
    }
    if (state == ParserState::HaveRecord)
        records.push_back(r);
    return state;
}

int32_t DLTFileParser::recordsCount()
{
    return mRecordsCount;
}

DLTFileRecordIterator DLTFileParser::begin()
{
    return DLTFileRecordIterator(*this);
}

DLTFileRecordIterator DLTFileParser::end()
{
    return DLTFileRecordIterator(*this, DLTFileRecordIterator::MaxRecordNum);
}

ParserState DLTFileParser::processLastRecord(DLTFileRecord &record)
{
    auto hdr0 = (DltStorageHeader *) &buffer[nextMsgPosLocal];
    if (sstrstr((char*)hdr0->pattern, (char*)"DLT\1", 0, 4) != 0) {
        DltStandardHeader * hdr = (DltStandardHeader *) &buffer[nextMsgPosLocal+sizeof(DltStorageHeader)];
        length_t msgLen = DLT_BETOH_16(hdr->len) + 16;
        mRecordsCount++;
        record = {mRecordsCount, currentMsgPosGlobal, true, msgLen, (char*)hdr0, false};
        endOfRecords = true;
        return ParserState::HaveRecord;
    }
    return ParserState::EndOfFile;
}

bool DLTFileParser::nextCallWillRead()
{
    return nextMsgPosGlobal >= bytesReadTotal;
}

DLTFileRecordIterator::DLTFileRecordIterator(DLTFileParser &p, uint32_t recordNum) : parser(p)
{
    if (recordNum == 0) {
        parser.init();
        ++(*this);
    } else
        record.num = recordNum;
}

DLTFileRecord DLTFileRecordIterator::operator *() const
{
    return record;
}

bool DLTFileRecordIterator::operator ==(const DLTFileRecordIterator &other) const
{
    return record == other.record;
}

bool DLTFileRecordIterator::operator !=(const DLTFileRecordIterator &other) const
{
    return !(*this == other);
}

DLTFileRecordIterator &DLTFileRecordIterator::operator ++()
{
    if (record.num != MaxRecordNum) {
        auto state = parser.parse(record);
        while (state == ParserState::NeedMoreData)
            state = parser.parse(record);
        if (state != ParserState::HaveRecord) {
            record = {MaxRecordNum,0,true,0,nullptr};
        }
    }
    return *this;
}

bool DLTFileRecord::operator ==(const DLTFileRecord &other) const
{
    return this->num == other.num && this->offset == other.offset && this->good == other.good && this->length == other.length && this->msg == other.msg;
}

ParsedDLTRecord DLTFileRecord::parse()
{
    DLTRecordParser p;
    p.parseHeaders(*this);
    return p.extractRecord();
}

}
