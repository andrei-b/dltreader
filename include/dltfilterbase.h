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

namespace DLTReader {

class DLTFilterBase
{
public:
    bool virtual match(DLTFileRecord & record) = 0;
    bool virtual match(const DLTRecordSet & records) = 0;
 };

}

#endif // DLTFILTERBASE_H
