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

#include "indexer.h"

Indexer::Indexer(DLTFileParser &p) : parser(p)
{

}

Index Indexer::makeIndex()
{
    Index result;
    if (!parser.init()) {
        return result;
    }
    for (const auto & r : parser)
        result.push_back(r.offset);
    return result;
}

SparceIndex Indexer::makeFilteredIndex(const std::vector<std::shared_ptr<DLTFilterBase> > &filters)
{

}

SparceIndex Indexer::makeFilteredIndex(const DLTFilterBase &filter)
{

}

SparceIndex Indexer::narrowIndex(const DLTFilterBase &filter, const SparceIndex &source)
{

}
