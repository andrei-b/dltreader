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

#ifndef DLTRECORDPARSER_H
#define DLTRECORDPARSER_H

#include "dltfileparser.h"
#include "parseddltrecord.h"
#include <string>
#include <vector>
#include <memory>

namespace __internal {

extern "C" {
#include "dlt_common.h"
}

}

namespace DLTReader {



class DLTRecordParser
{
public:
    DLTRecordParser();
    ~DLTRecordParser();
    bool parseHeaders(const DLTFileRecord &record);
    char * payloadPointer();
    std::string payloadAsString() const;
    uint16_t payloadLength();
    TextId ecu() const;
    const char *ecuPtr() const;
    const char * apid() const;
    const char * ctid() const;
    void parseAll(ParsedDLTRecord & record);
private:
    __internal::DltStorageHeader * storageHeader = nullptr;
    __internal::DltStandardHeader * standardHeader = nullptr;
    __internal::DltExtendedHeader * extendedHeader = nullptr;
    __internal::DltStandardHeaderExtra headerExtra;
    uint32_t messageNumber;
    uint64_t offset;
    bool good;
    length_t payloadSize = 0;
    length_t headerSize = 0;
    char * payloadPtr;
};

}

#endif // DLTRECORDPARSER_H
