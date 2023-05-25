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

#include "dltfilerecord.h"
#include "dltfilterbase.h"
#include "parseddltrecord.h"
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
class TextIdFilter: public DLTTextFilter
{
public:
    TextIdFilter(bool regEx, bool positiveFilter, const TextIdSet & set);
    bool virtual match(DLTFileRecord &record) const override;
    bool virtual match(const DLTRecordSet & records) override;
private:
    bool positiveFilter = true;
    TextIdSet set;
    template <TextIDField f = F, std::enable_if_t<(f == TextIDField::ApId)>* = nullptr>
    inline TextId getFiled(DLTFileRecord & record) const;
    template <TextIDField f = F, std::enable_if_t<(f == TextIDField::CtId)>* = nullptr>
    inline TextId getFiled(DLTFileRecord & record) const;
    template <TextIDField f = F, std::enable_if_t<(f == TextIDField::Ecu)>* = nullptr>
    inline TextId getFiled(DLTFileRecord & record) const;

};

template class TextIdFilter<TextIDField::ApId>;
template class TextIdFilter<TextIDField::CtId>;
template class TextIdFilter<TextIDField::Ecu>;

using ApIdFilter = TextIdFilter<TextIDField::ApId>;
using CtIdFilter = TextIdFilter<TextIDField::CtId>;
using EcuFilter = TextIdFilter<TextIDField::Ecu>;

}

#endif // TEXTIDFILTER_H
