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

template<enum TextIDField F>
template <TextIDField f, std::enable_if_t<(f == TextIDField::CtId)>*>
TextIdFilter<F>::TextIdFilter(bool positiveFilter, TextIdSet set) : positiveFilter(positiveFilter), set(set)
{

}

template<TextIDField F>
template <TextIDField f, std::enable_if_t<(f == TextIDField::ApId)>*>
bool TextIdFilter<F>::matchInternal(const DLTFileRecordRaw & record)
{
    DLTRecordParser rp;
    rp.parseHeaders(record);
    for (const auto & id : set) {
        if (id == rp.apid()) {
            return positiveFilter;
        }
    }
}

template<TextIDField F>
template <TextIDField f, std::enable_if_t<(f == TextIDField::CtId)>*>
bool TextIdFilter<F>::matchInternal(const DLTFileRecordRaw & record)
{
    DLTRecordParser rp;
    rp.parseHeaders(record);
    for (const auto & id : set) {
        if (id == rp.ctid()) {
            return positiveFilter;
        }
    }
}


template<TextIDField F>
bool TextIdFilter<F>::match(const DLTFileRecordRaw &record)
{
    return matchInternal<F>(record);
}



}
