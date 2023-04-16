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

#ifndef PARSEDRECORDSCOLLECTION_H
#define PARSEDRECORDSCOLLECTION_H

#include "dltrecordparser.h"
#include "dltfileparser.h"

namespace DLTFile {

class ParsedRecordIterator;
class ParsedRecordsCollection
{
public:
    explicit ParsedRecordsCollection(DLTFileParser & p);
    ParsedRecordIterator begin();
    ParsedRecordIterator end();
private:
    DLTFileParser & parser;
};

class ParsedRecordIterator
{
public:
    explicit ParsedRecordIterator(DLTFileParser * p = nullptr);
    typedef DLTFileRecordParsed value_type;
    typedef std::ptrdiff_t difference_type;
    typedef DLTFileRecordParsed * pointer;
    typedef DLTFileRecordParsed & reference;
    typedef std::input_iterator_tag iterator_category;
    DLTFileRecordParsed operator * () const;
    bool operator ==(const ParsedRecordIterator & other) const;
    bool operator !=(const ParsedRecordIterator & other) const;
    ParsedRecordIterator & operator ++ ();
private:
    DLTRecordParser recordParser;
    DLTFileParser * fileParser;
    DLTFileRecordParsed record;
};

}

#endif // PARSEDRECORDSCOLLECTION_H
