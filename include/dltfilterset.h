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

#ifndef DLTFILTERSET_H
#define DLTFILTERSET_H

#include "dltfilterbase.h"
#include <vector>
#include <memory>

namespace DLTReader {

class DLTFilterSet : public DLTFilterBase
{
public:
    explicit DLTFilterSet(std::shared_ptr<DLTFilterBase> filter);
    bool match(DLTFileRecord & record) const override;
    bool match(const DLTRecordSet & records) override;
    void andFilter(std::shared_ptr<DLTFilterBase> filter);
    void orFilter(std::shared_ptr<DLTFilterBase> filter);
private:
    enum class JoinType {
        None,
        And,
        Or
    };
    explicit DLTFilterSet(const std::vector<std::shared_ptr<DLTFilterBase> > &srcFilters, JoinType join);
    JoinType joinType = JoinType::None;
    std::vector<std::shared_ptr<DLTFilterBase>> filters;
};

}

#endif // DLTFILTERSET_H
