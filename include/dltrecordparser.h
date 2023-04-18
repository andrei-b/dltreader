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
#include <string>
#include <vector>
#include <memory>
extern "C" {
#include "dlt_common.h"
}

namespace DLTFile {

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
    bool operator ==(const TextId & other) const {
        return data[0] == other.data[0] && data[1] == other.data[1] && data[2] == other.data[2] && data[3] == other.data[3];
    }
    bool operator !=(const TextId & other) const {
        return !(*this == other);
    }
    bool operator ==(const char * other) const {
        return data[0] == other[0] && data[1] == other[1] && data[2] == other[2] && data[3] == other[3];
    }
    bool operator !=(const char * & other) const {
        return !(*this == other);
    }
    bool operator ==(const std::string & other) const {
        return *this == other.data();
    }
    bool operator !=(const std::string & other) const {
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

struct Payload {
    void set(const char * src, uint16_t length)
    {
       data.reset(new char[length + 64 - (length%64)]);
       len = length;
       std::copy(src, src+length, &data[0]);
    }
    std::shared_ptr<char[]> data;
    uint16_t len;
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
    uint32_t payloadSize;
    Payload payload;
};

class DLTRecordParser
{
public:
    DLTRecordParser();
    ~DLTRecordParser();
    bool parseHeaders(const DLTFileRecordRaw &record);
    void extractFileRecord(DLTFileRecordParsed & out);
private:
    DltStorageHeader * storageHeader = nullptr;
    DltStandardHeader * standardHeader = nullptr;
    DltExtendedHeader * extendedHeader = nullptr;
    DltStandardHeaderExtra headerExtra;
    uint32_t messageNumber;
    uint64_t offset;
    bool good;
    length_t payloadSize = 0;
    length_t headerSize = 0;
    char * payloadPtr;
};

}

#endif // DLTRECORDPARSER_H
