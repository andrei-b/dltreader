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

#ifndef DLTFILTERBASE_H
#define DLTFILTERBASE_H

#include "dltfileparser.h"
#include "indexer.h"

namespace DLTFile {

class DLTFilterBase
{
public:
    bool virtual match(const DLTFileRecordRaw & record) = 0;
    bool virtual match(const RecordCollection & records) = 0;
    template<typename T>
    SparceIndex apply(T begin, T end)
    {
        SparceIndex result;
        while (begin != end) {
            const DLTFileRecordRaw & r = *begin;
            if (match(r))
                result.records.push_back({r.offset, r.length});
            ++begin;
        }
        return result;
    }
};

}

#endif // DLTFILTERBASE_H
