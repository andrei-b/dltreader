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

namespace DLTReader {


class DLTIndexedRecordIterator;
class DLTRecordCollection
{
public:
    DLTRecordCollection(DLTFileParser & source, const DLTFilterBase & filter);
    explicit DLTRecordCollection(DLTFileParser & source);
    DLTRecordCollection(DLTRecordCollection & source, const DLTFilterBase & filter);
    DLTRecordCollection select(const DLTFilterBase & filter) const;
    DLTRecordCollection select(const std::string &payload, bool re) const;
    void filter(const DLTFilterBase & filter);
    void append(DLTRecordCollection & collection);
    void append(const DLTFileRecord & record);
    void append(const ParsedDLTRecord & record);
    DLTRecordCollection join(DLTRecordCollection & collection) const;
    DLTIndexedRecordIterator find(const DLTFilterBase & filter, DLTIndexedRecordIterator startFrom) const;
    DLTIndexedRecordIterator find(const std::string &payload, bool re, DLTIndexedRecordIterator startFrom) const;
    DLTIndexedRecordIterator begin() const;
    DLTIndexedRecordIterator end() const;
private:
    std::string fileName;
    SparceIndex index;
};


class DLTIndexedRecordIterator
{
public:
    static constexpr uint32_t MaxRecordNum = 0xFFFFFFFF;
    explicit DLTIndexedRecordIterator(DLTRecordCollection & p, uint32_t recordNum = 0);
    typedef DLTFileRecord value_type;
    typedef std::ptrdiff_t difference_type;
    typedef DLTFileRecord * pointer;
    typedef DLTFileRecord & reference;
    typedef std::random_access_iterator_tag iterator_category;
    DLTFileRecord operator * () const;
    bool operator ==(const DLTFileRecordIterator & other) const;
    bool operator !=(const DLTFileRecordIterator & other) const;

    DLTFileRecordIterator & operator ++ ();
    DLTFileRecordIterator & operator -- ();
    difference_type operator-(const DLTFileRecordIterator& it) const;
    DLTFileRecordIterator operator+(const difference_type& diff) const;
    DLTFileRecordIterator operator-(const difference_type& diff) const;

    static DLTFileRecordIterator makeEndIterator(DLTRecordCollection & p);
private:
    DLTRecordCollection & collection;
    DLTFileRecord record;
};

}

#endif // DLTRECORDCOLLECTION_H
