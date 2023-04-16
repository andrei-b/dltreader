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

#include "parsedrecordscollection.h"

namespace DLTFile {

ParsedRecordsCollection::ParsedRecordsCollection(DLTFileParser &p) : parser(p)
{

}

ParsedRecordIterator ParsedRecordsCollection::begin()
{
    return ParsedRecordIterator(&parser);
}

ParsedRecordIterator ParsedRecordsCollection::end()
{
    return ParsedRecordIterator();
}

ParsedRecordIterator::ParsedRecordIterator(DLTFileParser *p) : fileParser(p)
{
    if (fileParser != nullptr)
        fileParser->init();
    ++(*this);
}

DLTFileRecordParsed ParsedRecordIterator::operator *() const
{
    return record;
}

bool ParsedRecordIterator::operator ==(const ParsedRecordIterator &other) const
{
    return record.num == other.record.num && fileParser == other.fileParser;
}

bool ParsedRecordIterator::operator !=(const ParsedRecordIterator &other) const
{
    return !(*this == other);
}

ParsedRecordIterator &ParsedRecordIterator::operator ++()
{
    if (fileParser != nullptr) {
        DLTFileRecordRaw r;
        auto state = fileParser->parse(r);
        while (state == ParserState::NeedMoreData)
            state = fileParser->parse(r);
        if (state != ParserState::HaveRecord) {
            record.num = 0;
            fileParser = nullptr;
        } else {
            recordParser.parseHeaders(r);
            recordParser.extractFileRecord(record);
        }
    }
    return *this;
}

}
