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

#ifndef INDEXER_H
#define INDEXER_H
#include "dltfileparser.h"
#include "dltfilterbase.h"
#include <memory>
#include <vector>

namespace DLTFile {

using Index = std::vector<uint64_t>;
struct SparceRecord
{
    uint64_t offset = 0;
    uint16_t length = 0;
};
using SparceIndex = std::vector<SparceRecord>;

class Indexer
{
public:
    explicit Indexer(DLTFileParser & p);
    Index makeIndex();
    SparceIndex makeFilteredIndex(const std::vector<std::shared_ptr<DLTFilterBase>> & filters);
    SparceIndex makeFilteredIndex(const DLTFilterBase & filter);
    SparceIndex narrowIndex(const DLTFilterBase & filter, const SparceIndex & source);
private:
    DLTFileParser & parser;
};

}

#endif // INDEXER_H
