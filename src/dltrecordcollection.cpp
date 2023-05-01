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

DLTRecordCollection::DLTRecordCollection(DLTRecordCollection &source, const DLTFilterBase &filter) : mFileName(source.fileName())
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
