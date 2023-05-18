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

#ifndef PARSEDDLTRECORD_H
#define PARSEDDLTRECORD_H

#include "dltfilerecord.h"
#include <string>
#include <memory>
#include <ctime>
#include <iomanip>

namespace DLTReader
{

struct TextId {
    TextId(TextId & other)
    {
        *this = other;
    }
    TextId()
    {
    }
    TextId(const char * text)
    {
        *this = text;
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

const std::string DefaultTimeFormat = "%d-%m-%Y %H:%M:%S";

class ParsedDLTRecord
{
public:
    ParsedDLTRecord();
    explicit ParsedDLTRecord(const DLTFileRecord &record);
    void setPayload(const char * src, uint16_t length);
    std::string rawDataAsString();
    template <typename T>
    T payloadAs()
    {
        if (parsedPayload.size() == 0)
            parsePayload();
        return T(parsedPayload.begin(), parsedPayload.end());
    }
    uint32_t num() const;
    void setNum(uint32_t newNum);
    uint64_t offset() const;
    void setOffset(uint64_t newOffset);
    bool good() const;
    void setGood(bool newGood);
    TextId ecu();
    void setEcu(const TextId &newEcu);
    TextId apid();
    void setApid(const TextId &newApid);
    TextId ctid();
    void setCtid(const TextId &newCtid);
    DLTMessageType type() const;
    void setType(DLTMessageType newType);
    uint8_t subtype() const;
    void setSubtype(uint8_t newSubtype);
    DLTLogMode mode() const;
    void setMode(DLTLogMode newMode);
    uint32_t sessionId() const;
    void setSessionId(uint32_t newSessionId);
    uint32_t seconds() const;
    void setSeconds(uint32_t newSeconds);
    uint32_t microseconds() const;
    void setMicroseconds(uint32_t newMicroseconds);
    uint32_t timestamp() const;
    void setTimestamp(uint32_t newTimestamp);
    uint16_t payloadSize() const;
    void setPayloadSize(uint16_t newPayloadSize);
    struct std::tm wallTime(int32_t shift = 0);
    template <typename T>
    T timeAs(const std::string& format, int32_t shift = 0)
    {
        auto tm = wallTime(shift);
        std::ostringstream oss;
        oss << std::put_time(&tm, format.data());
        auto s = oss.str();
        return T(s.begin(), s.end());
    }
private:
    void parsePayload();
    uint32_t mNum = 0;
    uint64_t mOffset = 0;
    bool mGood = true;
    TextId mEcu;
    TextId mApid;
    TextId mCtid;
    DLTMessageType mType;
    uint8_t mSubtype;
    DLTLogMode mMode;
    uint32_t mSessionId;
    uint32_t mSeconds = 0;
    uint32_t mMicroseconds = 0;
    uint32_t mTimestamp = 0;
    uint16_t mPayloadSize;
    std::shared_ptr<char[]> data;
    std::u32string parsedPayload;
};


}

#endif // PARSEDDLTRECORD_H
