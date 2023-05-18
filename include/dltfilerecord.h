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

#ifndef DLTFILERECORD_H
#define DLTFILERECORD_H

#include <string>
#include <vector>

namespace DLTReader {

using length_t = uint16_t;

struct DLTFileRecord
{
    uint32_t num = 0;
    uint64_t offset = 0;
    bool good = true;
    length_t length = 0;
    char * msg = nullptr;
    bool headerParsed = false;
    const char * ctid;
    const char * apid;
    const char * ecu;
    uint16_t payloadLength;
    const char * payload;
    bool operator == (const DLTFileRecord & other) const;
    void lightParse();
};

using DLTRecordSet = std::vector<DLTFileRecord>;

}

#endif
