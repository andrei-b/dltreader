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
    buffer.reserve(BufferSize);
    index = selectInternal<DLTFileRecordIterator>(source.begin(), source.end(), filter);
}

DLTRecordCollection::DLTRecordCollection(DLTFileParser & source) : DLTRecordCollection(source.begin(), source.end())
{
    //buffer.reserve(BufferSize);
}

DLTRecordCollection::DLTRecordCollection(DLTRecordCollection &source, const DLTFilterBase &filter) : mFileName(source.fileName())
{
    buffer.reserve(BufferSize);
    index = selectInternal<DLTIndexedRecordIterator>(source.begin(), source.end(), filter);
}

DLTRecordCollection::DLTRecordCollection(DLTFileRecordIterator begin, DLTFileRecordIterator end) : mFileName(begin.fileName())
{
    buffer.reserve(BufferSize);
    while(begin != end) {
        auto r = *begin;
        index.records.push_back({r.num, r.offset, r.length, r.good});
        ++begin;
    }
}

DLTRecordCollection DLTRecordCollection::select(const DLTFilterBase &filter)
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
    index.records.insert(i, {record.num, record.offset, record.length, record.good});
}

void DLTRecordCollection::append(const ParsedDLTRecord &record)
{
    auto i = index.records.begin();
    while (i < index.records.end() && i->offset < record.offset) ++i;
    index.records.insert(i, {record.num, record.offset, record.payloadSize, record.good});
}

DLTRecordCollection DLTRecordCollection::join(DLTRecordCollection &collection) const
{
    return DLTRecordCollection(merge(index, collection.index), mFileName);
}

DLTIndexedRecordIterator DLTRecordCollection::find(const DLTFilterBase &filter, DLTIndexedRecordIterator startFrom)
{
    while(startFrom != end()) {
        auto r = *startFrom;
        if (filter.match(r))
            return startFrom;
        ++startFrom;
    }
    return end();
}

DLTIndexedRecordIterator DLTRecordCollection::find(const std::string &payload, bool re, DLTIndexedRecordIterator startFrom)
{

}

DLTIndexedRecordIterator DLTRecordCollection::begin()
{
    return DLTIndexedRecordIterator(*this);
}

DLTIndexedRecordIterator DLTRecordCollection::end()
{
    return DLTIndexedRecordIterator::makeEndIterator(*this);
}

std::string DLTRecordCollection::fileName() const
{
    return mFileName;
}

uint32_t DLTRecordCollection::recordCount() const
{
    return index.records.size();
}

bool DLTRecordCollection::fileOpen()
{
    bufferOffset = 0;
    bytesInBuffer = 0;
    if (!file.is_open())
        file.open(fileName());
    if (file.is_open()) {
        return true;
    }
    return false;
}

bool DLTRecordCollection::isFileOpen()
{
    return file.is_open();
}

DLTFileRecord DLTRecordCollection::fileRead(uint32_t indexPosition, bool forward)
{
    if (indexPosition >= index.records.size())
        return {indexPosition, 0, false, 0, nullptr};
    if (index.records[indexPosition].offset < bufferOffset || index.records[indexPosition].offset >= bufferOffset + bytesInBuffer) {
        if (forward) {
            uint32_t i = indexPosition;
            while (i + 1 < index.records.size() && index.records[i+1].offset - index.records[indexPosition].offset + index.records[i+1].length  < BufferSize)
                ++i;
            bufferOffset = index.records[indexPosition].offset;
            bytesInBuffer = index.records[i].offset - index.records[indexPosition].offset + index.records[i].length;
        } else {
            uint32_t i = indexPosition;
            while (i > 0 && index.records[indexPosition].offset + index.records[indexPosition].length - index.records[i-1].offset  < BufferSize)
                --i;
            bufferOffset = index.records[i].offset;
            bytesInBuffer = index.records[indexPosition].offset + index.records[indexPosition].length - index.records[i].offset;
        }
        file.seekg(bufferOffset);
        file.read(buffer.data(), bytesInBuffer);
    }
    return {index.records[indexPosition].srcNum, index.records[indexPosition].offset, index.records[indexPosition].good, index.records[indexPosition].length, buffer.data() + index.records[indexPosition].offset - bufferOffset};
}

ParsedDLTRecord DLTRecordCollection::getRecord(uint32_t indexPosition)
{
    return fileRead(indexPosition).parse();
}

void DLTRecordCollection::fileClose()
{
    file.close();
}

DLTRecordCollection::DLTRecordCollection(SparceIndex &&index, const std::string &fileName) : index(std::move(index)), mFileName(fileName)
{
    buffer.reserve(BufferSize);
}

template<typename Iterator>
SparceIndex DLTRecordCollection::selectInternal(Iterator begin, Iterator end, const DLTFilterBase &filter)
{
    SparceIndex result;
    while(begin != end) {
        DLTFileRecord r = *begin;
        DLTFileRecord & rr = r;
        if (filter.match(rr))
            result.records.push_back({r.num, r.offset, r.length, r.good});
        ++begin;
    }
    return result;
}

DLTIndexedRecordIterator::DLTIndexedRecordIterator(DLTRecordCollection &collection) : collection(collection)
{
    collection.fileOpen();
    record = collection.fileRead(0);
}

DLTFileRecord DLTIndexedRecordIterator::operator *() const
{
    return record;
}

bool DLTIndexedRecordIterator::operator ==(const DLTIndexedRecordIterator &other) const
{
    return currentIndex == other.currentIndex;
}

bool DLTIndexedRecordIterator::operator <(const DLTIndexedRecordIterator &other) const
{
    return currentIndex < other.currentIndex;
}

bool DLTIndexedRecordIterator::operator >(const DLTIndexedRecordIterator &other) const
{
    return currentIndex > other.currentIndex;
}

DLTIndexedRecordIterator &DLTIndexedRecordIterator::operator ++()
{
    if (currentIndex < collection.recordCount())
        currentIndex++;
    record = collection.fileRead(currentIndex);
    return *this;
}

DLTIndexedRecordIterator &DLTIndexedRecordIterator::operator --()
{
    if (currentIndex > 0)
        currentIndex--;
    record = collection.fileRead(currentIndex, false);
    return *this;
}

DLTIndexedRecordIterator::difference_type DLTIndexedRecordIterator::operator-(const DLTIndexedRecordIterator &other)
{
    return currentIndex-other.currentIndex;
}

DLTIndexedRecordIterator DLTIndexedRecordIterator::operator-(const difference_type &diff)
{
    if (currentIndex >= diff)
        currentIndex -= diff;
    else
        currentIndex = 0;
    record = collection.fileRead(currentIndex, false);
    return *this;
}

DLTIndexedRecordIterator DLTIndexedRecordIterator::operator+(const difference_type &diff)
{
    if (currentIndex + diff <= collection.recordCount())
        currentIndex += diff;
    else currentIndex = collection.recordCount();
    record = collection.fileRead(currentIndex);
    return *this;
}

DLTIndexedRecordIterator DLTIndexedRecordIterator::makeEndIterator(DLTRecordCollection &c)
{
    DLTIndexedRecordIterator result(c);
    result.currentIndex = c.recordCount();
    return result;
}

std::string DLTIndexedRecordIterator::fileName() const
{
    return collection.fileName();
}

}
