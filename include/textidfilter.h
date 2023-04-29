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
#include "parseddltrecord.h"
#include "dltrecordparser.h"
#include <vector>

namespace DLTReader
{

using TextIdSet = std::vector<TextId>;


enum class TextIDField
{
    ApId = 0,
    CtId,
    Ecu
};

template<enum TextIDField F>
class TextIdFilter: public DLTFilterBase
{
public:
    template <TextIDField f = F, std::enable_if_t<(f == TextIDField::CtId)>* = nullptr>
    TextIdFilter(bool positiveFilter, TextIdSet set);

    template <TextIDField f = F, std::enable_if_t<(f == TextIDField::ApId)>* = nullptr>
    bool matchInternal(const DLTFileRecordRaw & record);

    template <TextIDField f = F, std::enable_if_t<(f == TextIDField::CtId)>* = nullptr>
    bool matchInternal(const DLTFileRecordRaw & record);

    bool virtual match(const DLTFileRecordRaw & record) override;
    bool virtual match(const RecordCollection & records) override;
private:
    bool positiveFilter = true;
    TextIdSet set;
};

}

#endif // TEXTIDFILTER_H
