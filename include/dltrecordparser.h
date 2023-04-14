/*
    DLTReader Diagnostic Log and Trace reading library
    Copyright (C) 2023 Andrei Borovsky <andrey.borovsky@gmail.com>
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
#include <string>
#include <vector>
#include <memory>

DLTFileRecordRaw & parseFileRecordHeaders(DLTFileRecordRaw & record);

struct TextId {
    char data[5] = {'\0','\0','\0','\0','\0'};
    operator std::string(){
        return std::string(data, data+5);
    }
    TextId & operator =(const TextId & other) {
        data[0] = other.data[0];
        data[1] = other.data[1];
        data[2] = other.data[2];
        data[3] = other.data[3];
        return (*this);
    }
    TextId & operator =(const char * other) {
        data[0] = other[0];
        data[1] = other[1];
        data[2] = other[2];
        data[3] = other[3];
        return (*this);
    }
    TextId & operator =(const std::string & other) {
        (*this) = other.data();
        return (*this);
    }
    bool operator ==(const TextId & other) {
        return data[0] == other.data[0] && data[1] == other.data[1] && data[2] == other.data[2] && data[3] == other.data[3];
    }
    bool operator !=(const TextId & other) {
        return !(*this == other);
    }
    bool operator ==(const char * other) {
        return data[0] == other[0] && data[1] == other[1] && data[2] == other[2] && data[3] == other[3];
    }
    bool operator !=(const char * & other) {
        return !(*this == other);
    }
    bool operator ==(const std::string & other) {
        return *this == other.data();
    }
    bool operator !=(const std::string & other) {
        return !(*this == other);
    }
};

enum class DLTMessageType {
    DltTypeUnknown = -2,
    DltTypeLog = 0,
    DltTypeAppTrace,
    DltTypeNwTrace,
    DltTypeControl
};

enum class DLTLogMode {
    Verbose,
    NonVerbose
};

struct DLTFileRecordParsed
{
    uint32_t num = 0;
    uint64_t offset = 0;
    bool good = true;
    TextId ecu;
    TextId apid;
    TextId ctid;
    DLTMessageType type;
    uint8_t subtype;
    DLTLogMode mode;
    uint32_t sessionId;
    uint32_t seconds = 0;
    uint32_t microseconds = 0;
    uint32_t timestamp = 0;
    std::shared_ptr<std::vector<char>> payloadPtr;
};

class DLTRecordParser
{
public:
    DLTRecordParser();
    bool parseHeaders(const DLTFileRecordRaw &record);
    void extractFileRecord(DLTFileRecordParsed & out);
private:
    uint32_t messageNumber;
    uint64_t offset;
    bool good;
    DltStorageHeader * storageHeader = nullptr;
    DltStandardHeader * standardHeader = nullptr;
    DltExtendedHeader * extendedHeader = nullptr;
    DltStandardHeaderExtra headerExtra;
    length_t payloadSize = 0;
    length_t headerSize = 0;
    std::shared_ptr<std::vector<char>> payloadPtr;
};

#endif // DLTRECORDPARSER_H
