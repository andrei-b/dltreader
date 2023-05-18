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

#ifndef DLTFILEPARSER_H
#define DLTFILEPARSER_H

#include "dltfilerecord.h"
#include <string>
#include <vector>
#include <iterator>

namespace DLTReader {
using BufPtr = char*;

enum class ParserState {
    HaveRecord = 0,
    NeedMoreData,
    EndOfFile,
    Error
};

class DLTFileRecordIterator;
class DLTFileParser
{
public:
    explicit DLTFileParser(const std::string & fileName);
    DLTFileParser(const DLTFileParser & other) = delete;
    DLTFileParser(DLTFileParser && other);
    DLTFileParser & operator = (const DLTFileParser & other) = delete;
    DLTFileParser & operator = (DLTFileParser && other) = delete;
    virtual ~DLTFileParser();
    const std::string &fileName();
    virtual bool init();
    virtual void reset();
    ParserState parse(DLTFileRecord & record);
    ParserState parseBatch(DLTRecordSet & records);
    int32_t recordsCount();
    DLTFileRecordIterator begin();
    DLTFileRecordIterator end();
protected:
    virtual bool resetFile() = 0;
    virtual uint64_t readFile(BufPtr &buf) = 0;
    virtual void seek(uint64_t pos) = 0;
    virtual void closeFile() = 0;
private:
    bool endOfRecords = false;
    uint32_t mRecordsCount = 0;
    std::string mFileName;
    uint64_t currentMsgPosGlobal = 0;
    uint64_t nextMsgPosGlobal = 0;
    uint64_t bytesReadTotal = 0;
    uint32_t bytesRead = 0;
    int32_t currentMsgPosLocal = 0;
    uint32_t nextMsgPosLocal = 0;
    uint64_t localMsgOffset = 0;
    BufPtr buffer = nullptr;
    ParserState processLastRecord(DLTFileRecord & record);
    bool nextCallWillRead();
};

class DLTFileRecordIterator
{
public:
    static constexpr uint32_t MaxRecordNum = 0xFFFFFFFF;
    explicit DLTFileRecordIterator(DLTFileParser & p, uint32_t recordNum = 0);
    typedef DLTFileRecord value_type;
    typedef std::ptrdiff_t difference_type;
    typedef DLTFileRecord * pointer;
    typedef DLTFileRecord & reference;
    typedef std::input_iterator_tag iterator_category;
    DLTFileRecord operator * () const;
    bool operator ==(const DLTFileRecordIterator & other) const;
    bool operator !=(const DLTFileRecordIterator & other) const;
    DLTFileRecordIterator & operator ++ ();
    static DLTFileRecordIterator makeEndIterator(DLTFileParser & p);
    std::string fileName() const;
private:
    DLTFileParser & parser;
    DLTFileRecord record;
};

}

#endif // DLTFILEPARSER_H
