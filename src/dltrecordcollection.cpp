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

#include "dltrecordcollection.h"
#include <algorithm>

namespace DLTReader {


DLTRecordCollection::DLTRecordCollection(DLTFileParser &source, const DLTFilterBase &filter) : mFileName(source.fileName())
{
    index = selectInternal<DLTFileRecordIterator>(source.begin(), source.end(), filter);
}

DLTRecordCollection::DLTRecordCollection(DLTFileParser & source) : DLTRecordCollection(source.begin(), source.end())
{

}

DLTRecordCollection::DLTRecordCollection(const DLTRecordCollection &source, const DLTFilterBase &filter) : mFileName(source.fileName())
{
    index = selectInternal<DLTIndexedRecordIterator>(source.begin(), source.end(), filter);
}

DLTRecordCollection::DLTRecordCollection(DLTFileRecordIterator begin, DLTFileRecordIterator end) : mFileName(begin.fileName())
{
    while(begin != end) {
        auto r = *begin;
        index.records.push_back({r.num, r.offset, r.length});
        ++begin;
    }
}

DLTRecordCollection DLTRecordCollection::select(const DLTFilterBase &filter) const
{
    return DLTRecordCollection(*this, filter);
}

void DLTRecordCollection::filter(const DLTFilterBase &filter)
{
    index = selectInternal<DLTIndexedRecordIterator>(begin(), end(), filter);
}

void DLTRecordCollection::append(DLTRecordCollection &collection)
{
    index = merge(index, collection.index);
}

void DLTRecordCollection::append(const DLTFileRecord &record)
{
    auto i = index.records.begin();
    while (i < index.records.end() && i->offset < record.offset) ++i;
    index.records.insert(i, {record.num, record.offset, record.length});
}

void DLTRecordCollection::append(const ParsedDLTRecord &record)
{
    auto i = index.records.begin();
    while (i < index.records.end() && i->offset < record.offset) ++i;
    index.records.insert(i, {record.num, record.offset, record.payloadSize});
}

DLTRecordCollection DLTRecordCollection::join(DLTRecordCollection &collection) const
{
    return DLTRecordCollection(merge(index, collection.index), mFileName);
}

DLTIndexedRecordIterator DLTRecordCollection::find(const DLTFilterBase &filter, DLTIndexedRecordIterator startFrom) const
{
    while(startFrom != end()) {
        auto r = *startFrom;
        if (filter.match(r))
            return startFrom;
        ++startFrom;
    }
    return end();
}

DLTIndexedRecordIterator DLTRecordCollection::find(const std::string &payload, bool re, DLTIndexedRecordIterator startFrom) const
{

}

DLTIndexedRecordIterator DLTRecordCollection::begin() const
{
    return DLTIndexedRecordIterator(*this);
}

DLTIndexedRecordIterator DLTRecordCollection::end() const
{
    return DLTIndexedRecordIterator::makeEndIterator(*this);
}

std::string DLTRecordCollection::fileName() const
{
    return mFileName;
}

DLTRecordCollection::DLTRecordCollection(SparceIndex &&index, const std::string &fileName) : index(std::move(index)), mFileName(fileName)
{

}

template<typename Iterator>
SparceIndex DLTRecordCollection::selectInternal(Iterator begin, Iterator end, const DLTFilterBase &filter)
{
    SparceIndex result;
    while(begin != end) {
        DLTFileRecord r = *begin;
        DLTFileRecord & rr = r;
        if (filter.match(rr))
            result.records.push_back({r.num, r.offset, r.length});
        ++begin;
    }
    return result;
}

}
