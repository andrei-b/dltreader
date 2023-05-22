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

#include "textidfilter.h"
#include "dltrecordparser.h"

namespace DLTReader {

template<TextIDField F>
bool TextIdFilter<F>::operator()(DLTFileRecord &record) const
{
    return match(record);
}

template<TextIDField F>
TextIdFilter<F>::TextIdFilter(bool positiveFilter, const TextIdSet &set): positiveFilter(positiveFilter), set(set)
{

}

template<TextIDField F>
bool TextIdFilter<F>::match(DLTFileRecord &record) const
{
    if (!record.headerParsed)
        record.lightParse();
    for (const auto & id : set) {
        if (id == getFiled<F>(record)) {
            return positiveFilter;
        }
    }
    return !positiveFilter;
}

template<TextIDField F>
bool TextIdFilter<F>::match(const DLTRecordSet &records)
{

}

template<TextIDField F>
template <TextIDField f, std::enable_if_t<(f == TextIDField::ApId)>*>
TextId TextIdFilter<F>::getFiled(DLTFileRecord &record) const
{
    return record.apid;
}

template<TextIDField F>
template <TextIDField f, std::enable_if_t<(f == TextIDField::CtId)>*>
TextId TextIdFilter<F>::getFiled(DLTFileRecord &record) const
{
    return record.ctid;
}

template<TextIDField F>
template <TextIDField f, std::enable_if_t<(f == TextIDField::Ecu)>*>
TextId TextIdFilter<F>::getFiled(DLTFileRecord &record) const
{
    return record.ecu;
}

}
