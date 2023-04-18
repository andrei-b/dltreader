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

#include "parsedrecordscollection.h"

namespace DLTFile {

ParsedRecordsCollection::ParsedRecordsCollection(DLTFileParser &p) : parser(p)
{

}

ParsedRecordIterator ParsedRecordsCollection::begin()
{
    return ParsedRecordIterator(parser);
}

ParsedRecordIterator ParsedRecordsCollection::end()
{
    return ParsedRecordIterator(parser, true);
}

ParsedRecordIterator::ParsedRecordIterator(DLTFileParser &p, bool end) : fileParser(p), atEnd(end)
{
    if (!atEnd) {
        fileParser.init();
        ++(*this);
    }
}

DLTFileRecordParsed ParsedRecordIterator::operator *() const
{
    return record;
}

bool ParsedRecordIterator::operator ==(const ParsedRecordIterator &other) const
{
    return record.num == other.record.num && atEnd == other.atEnd;
}

bool ParsedRecordIterator::operator !=(const ParsedRecordIterator &other) const
{
    return !(*this == other);
}

ParsedRecordIterator &ParsedRecordIterator::operator ++()
{
    if (!atEnd) {
        DLTFileRecordRaw r;
        auto state = fileParser.parse(r);
        while (state == ParserState::NeedMoreData)
            state = fileParser.parse(r);
        if (state != ParserState::HaveRecord) {
            record.num = 0;
            atEnd = true;
        } else {
            recordParser.parseHeaders(r);
            recordParser.extractFileRecord(record);
        }
    }
    return *this;
}

}
