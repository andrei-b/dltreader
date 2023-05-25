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

#include "dltfilterset.h"

namespace DLTReader {

DLTFilterSet::DLTFilterSet(std::shared_ptr<DLTFilterBase> filter)
{
    filters.push_back(filter);
}

bool DLTFilterSet::match(DLTFileRecord &record) const
{
    if (joinType == JoinType::And) {
        for (const auto & f : filters)
            if (!f->match(record))
                return false;
        return true;
    } else {
        for (const auto & f : filters)
            if (f->match(record))
                return true;
        return false;
    }
}

void DLTFilterSet::andFilter(std::shared_ptr<DLTFilterBase> filter)
{
    if (joinType == JoinType::Or) {
        std::shared_ptr<DLTFilterSet> newFilter(new DLTFilterSet(filters, joinType));
        filters.clear();
        filters.push_back(newFilter);
    }
    filters.push_back(filter);
    joinType = JoinType::And;
}

void DLTFilterSet::orFilter(std::shared_ptr<DLTFilterBase> filter)
{
    if (joinType == JoinType::And) {
        std::shared_ptr<DLTFilterSet> newFilter(new DLTFilterSet(filters, joinType));
        filters.clear();
        filters.push_back(newFilter);
    }
    filters.push_back(filter);
    joinType = JoinType::Or;
}

DLTFilterSet::DLTFilterSet(const std::vector<std::shared_ptr<DLTFilterBase> > &srcFilters, JoinType join) : joinType(join)
{
    filters.assign(srcFilters.begin(), srcFilters.end());
}

}
