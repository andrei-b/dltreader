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

TextIdFilter::TextIdFilter(bool includeFilter, TextIdSet ctid, TextIdSet apid, TextIdSet ecu) : includeFilter(includeFilter), ctid(ctid.begin(), ctid.end()), apid(apid.begin(), apid.end()), ecu(ecu.begin(), ecu.end())
{

}

bool TextIdFilter::match(const DLTFileRecordRaw &record)
{
    DLTRecordParser rp;
    rp.parseHeaders(record);
    for (const auto & id : ctid) {
        if (id == rp.ctid()) {
            return includeFilter;
        }
    }
    for (const auto & id : apid) {
        if (id == rp.apid()) {
            return includeFilter;
        }
    }
    for (const auto & id : ecu) {
        if (id == rp.ecu()) {
            return includeFilter;
        }
    }
    return !includeFilter;
}

bool TextIdFilter::match(const RecordCollection &records)
{

}

}
