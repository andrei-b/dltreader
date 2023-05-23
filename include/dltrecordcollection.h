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

#ifndef DLTRECORDCOLLECTION_H
#define DLTRECORDCOLLECTION_H

#include "dltfilterbase.h"
#include "dltfileparser.h"
#include "indexer.h"
#include "parseddltrecord.h"
#include <string>
#include<iostream>
#include<fstream>

namespace DLTReader {


class DLTIndexedRecordIterator;
class DLTRecordCollection
{
public:
    DLTRecordCollection(DLTFileParser & source, const DLTFilterBase & filter);
    explicit DLTRecordCollection(DLTFileParser & source);
    DLTRecordCollection(DLTRecordCollection &source, const DLTFilterBase & filter);
    DLTRecordCollection(DLTFileRecordIterator begin, DLTFileRecordIterator end);
    DLTRecordCollection(DLTIndexedRecordIterator begin, DLTIndexedRecordIterator end);
    DLTRecordCollection select(const DLTFilterBase & filter);
    DLTRecordCollection select(const std::string &payload, bool re) const;
    DLTRecordCollection select(uint32_t startNum, uint32_t endNum);
    void filter(const DLTFilterBase & filter);
    void append(DLTRecordCollection & collection);
    void append(const DLTFileRecord & record);
    void append(const ParsedDLTRecord & record);
    DLTRecordCollection join(DLTRecordCollection & collection) const;
    DLTIndexedRecordIterator find(const DLTFilterBase & filter, DLTIndexedRecordIterator startFrom);
    DLTIndexedRecordIterator find(const std::string &payload, bool re, DLTIndexedRecordIterator startFrom) ;
    DLTIndexedRecordIterator find(uint32_t recordNum);
    DLTIndexedRecordIterator begin();
    DLTIndexedRecordIterator end();
    std::string fileName() const;
    uint32_t recordCount() const;
    bool fileOpen();
    bool isFileOpen();
    DLTFileRecord fileRead(uint32_t indexPosition, bool forward = true);
    ParsedDLTRecord getRecord(uint32_t indexPosition);
    void fileClose();
private:
    static constexpr uint32_t BufferSize = 1024*1024;
    std::vector<char> buffer;
    uint64_t bufferOffset = 0;
    uint32_t bytesInBuffer = 0;
    DLTRecordCollection(SparceIndex && index, const std::string & fileName);
    template <typename Iterator>
    SparceIndex selectInternal(Iterator begin, Iterator end, const DLTFilterBase & filter);
    uint32_t firstRecNotLessThanNumToIndex(uint32_t recNum);
    std::string mFileName;
    SparceIndex index;
    std::ifstream file;
};


class DLTIndexedRecordIterator
{
public:
    explicit DLTIndexedRecordIterator(DLTRecordCollection &collection, uint32_t recordIndex = 0);
    typedef DLTFileRecord value_type;
    typedef uint64_t difference_type;
    typedef DLTFileRecord * pointer;
    typedef DLTFileRecord & reference;
    typedef std::random_access_iterator_tag iterator_category;
    DLTFileRecord operator * () const;
    bool operator == (const DLTIndexedRecordIterator & other) const;
    bool operator != (const DLTIndexedRecordIterator & other) const {return !(*this == other);}
    bool operator < (const DLTIndexedRecordIterator & other) const;
    bool operator > (const DLTIndexedRecordIterator & other) const;
    bool operator >= (const DLTIndexedRecordIterator & other) const { return !(*this <  other); }
    bool operator <= (const DLTIndexedRecordIterator & other) const { return !(*this > other); }

    DLTIndexedRecordIterator &operator ++();
    DLTIndexedRecordIterator & operator -- ();
    difference_type operator-(const DLTIndexedRecordIterator& other) ;
    DLTIndexedRecordIterator operator+(const difference_type& diff);
    DLTIndexedRecordIterator operator-(const difference_type& diff);

    std::string fileName() const;
private:
    DLTRecordCollection & collection;
    uint32_t currentIndex = 0;
    DLTFileRecord record;
};

}

#endif // DLTRECORDCOLLECTION_H
