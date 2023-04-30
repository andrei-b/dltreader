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

ApIdFilter::ApIdFilter(bool positive, TextIdSet &&set) : positive(positive), set(std::move(set))
{

}

bool ApIdFilter::match(DLTFileRecord &record)
{
    if (!record.headerParsed)
        record.lightParse();
    for (const auto & id : set) {
        if (id == record.apid) {
            return positive;
        }
    }
    return !positive;
}

bool ApIdFilter::match(const DLTRecordSet &records)
{

}

CtIdFilter::CtIdFilter(bool positive, TextIdSet &&set) : positive(positive), set(std::move(set))
{

}

bool CtIdFilter::match(DLTFileRecord &record)
{
    if (!record.headerParsed)
        record.lightParse();
    for (const auto & id : set) {
        if (id == record.ctid) {
            return positive;
        }
    }
    return !positive;
}

bool CtIdFilter::match(const DLTRecordSet &records)
{

}

EcuFilter::EcuFilter(bool positive, TextIdSet &&set) : positive(positive), set(std::move(set))
{

}

bool EcuFilter::match(DLTFileRecord &record)
{
    if (!record.headerParsed)
        record.lightParse();
    for (const auto & id : set) {
        if (id == record.ecu) {
            return positive;
        }
    }
    return !positive;
}

bool EcuFilter::match(const DLTRecordSet &records)
{

}

}
