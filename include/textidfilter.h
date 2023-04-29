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

#ifndef TEXTIDFILTER_H
#define TEXTIDFILTER_H

#include "dltfilterbase.h"
#include "parseddltrecord.h"
#include <vector>

namespace DLTReader
{

 using TextIdSet = std::vector<TextId>;


class TextIdFilter: public DLTFilterBase
{
public:
    explicit TextIdFilter(bool includeFilter, TextIdSet ctid, TextIdSet apid = TextIdSet(), TextIdSet ecu = TextIdSet());
    bool virtual match(const DLTFileRecord & record) override;
    bool virtual match(const RecordCollection & records) override;
private:
    bool includeFilter = true;
    TextIdSet ctid;
    TextIdSet apid;
    TextIdSet ecu;
};

}

#endif // TEXTIDFILTER_H
