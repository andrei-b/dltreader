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

struct TextId {
    TextId(TextId & other)
    {
        *this = other;
    }
    TextId()
    {
    }
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
    TextId & operator =(const std::string & other)
    {
        (*this) = other.data();
        return (*this);
    }
    bool operator ==(const TextId & other) const
    {
        return data[0] == other.data[0] && data[1] == other.data[1] && data[2] == other.data[2] && data[3] == other.data[3];
    }
    bool operator ==(const char * other) const
    {
        if (other != nullptr)
            return data[0] == other[0] && data[1] == other[1] && data[2] == other[2] && data[3] == other[3];
        return false;
    }
    bool operator ==(const std::string & other) const
    {
        return *this == other.data();
    }
    template<typename T>
    bool operator !=(const T & other) const
    {
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


struct ParsedDLTRecord
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
    uint16_t payloadSize;
    void set(const char * src, uint16_t length)
    {
       data.reset(new char[length]);
       payloadSize = length;
       std::copy(src, src+payloadSize, data.get());
    }
    std::shared_ptr<char[]> data;
};

class DLTRecordParser
{
public:
    DLTRecordParser();
    ~DLTRecordParser();
    bool parseHeaders(const DLTFileRecordRaw &record);
    char * payloadPointer();
    uint16_t payloadLength();
    TextId ecu() const;
    const char * apid() const;
    const char * ctid() const;
    ParsedDLTRecord extractRecord();
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
