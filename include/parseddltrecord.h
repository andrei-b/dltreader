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
#include "frozen/map.h"
#include "frozen/string.h"
#include <string>
#include <memory>
#include <ctime>
#include <iomanip>
#include <map>

namespace DLTReader
{

struct TextId {
    TextId(const TextId & other)
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
    Unknown = -2,
    Log = 0,
    AppTrace,
    NwTrace,
    Control
};

constexpr frozen::map<DLTMessageType, frozen::string, 5> dltMessageTypeToString = {
    {DLTMessageType::Unknown, "unknown"},
    {DLTMessageType::Log, "log"},
    {DLTMessageType::Control, "control"},
    {DLTMessageType::AppTrace, "app trace"},
    {DLTMessageType::NwTrace, "nw trace"}
};

enum class DLTLogMode {
    Verbose,
    NonVerbose
};

constexpr frozen::map<DLTLogMode, frozen::string, 2> dltLogModeToString = {
    {DLTLogMode::Verbose, "verbose"},
    {DLTLogMode::NonVerbose, "non-verbose"},
};

class ParsedDLTRecord
{
public:
    static constexpr char DefaultTimeFormat[] = "%d-%m-%Y %H:%M:%S.{microseconds}";
    ParsedDLTRecord();
    explicit ParsedDLTRecord(const DLTFileRecord &record);
    void setPayload(const char * src, uint16_t length);
    std::string rawDataAsString();
    template <typename StringT>
    StringT payloadAsString(bool whitewashNewlines = false)
    {
        if (parsedPayload.size() == 0)
            parsePayload();
        std::u32string newLine = U"\n";
        std::u32string newLineReplace = U"\\n";
        std::u32string cReturn = U"\r";
        std::u32string cReturnReplace = U"\\r";
        if (whitewashNewlines) {
            auto tmpPalyload = parsedPayload;
            auto pos = tmpPalyload.find(newLine);
            while(pos != std::u32string::npos) {
                tmpPalyload.replace(pos, newLine.size(), newLineReplace);
                pos = tmpPalyload.find(newLine, pos);
            }
            pos = tmpPalyload.find(cReturn);
            while(pos != std::u32string::npos) {
                tmpPalyload.replace(pos, cReturn.size(), cReturnReplace);
                pos = tmpPalyload.find(cReturn);
            }
            return StringT(tmpPalyload.begin(), tmpPalyload.end());
        }
        return StringT(parsedPayload.begin(), parsedPayload.end());
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
    template <typename StringT>
    StringT typeToString ()
    {
        auto s = dltMessageTypeToString.at(type());
        return StringT(s.begin(), s.end());
    }
    template <typename StringT>
    StringT modeToString ()
    {
        auto s = dltLogModeToString.at(mode());
        return StringT(s.begin(), s.end());
    }
    template <typename StringT>
    StringT apidToString ()
    {
        return textidToString<StringT>(apid());
    }
    template <typename StringT>
    StringT ctidToString ()
    {
        return textidToString<StringT>(ctid());
    }
    template <typename StringT>
    StringT ecuToString ()
    {
        return textidToString<StringT>(ecu());
    }
    template <typename StringT>
    StringT walltimeToString (const std::string& format, int32_t shift = 0)
    {
        auto tm = wallTime(shift);
        std::ostringstream oss;
        oss << std::put_time(&tm, format.data());
        auto s = oss.str();
        s = s.replace(s.find("{microseconds}"), 14, std::to_string(mMicroseconds));
        return StringT(s.begin(), s.end());
    }
    template <typename StringT>
    StringT recordToString (const std::string& format, const std::string& walltimeFormat, int32_t walltimeShift = 0, bool whitewashNewlines = false)
    {
        auto phNum = toString<StringT>("{num}");
        auto phWtime = toString<StringT>("{walltime}");
        auto phPayload = toString<StringT>("{payload}");
        auto phApid = toString<StringT>("{apid}");
        auto phCtid = toString<StringT>("{ctid}");
        auto phEcu = toString<StringT>("{ecu}");
        auto phTimestamp = toString<StringT>("{timestamp}");
        auto phMode = toString<StringT>("{mode}");
        auto phType = toString<StringT>("{type}");
        auto phSessId = toString<StringT>("{sessid}");
        auto wformat = toString<StringT>(format);
        replace(wformat, phPayload, payloadAsString<StringT>(whitewashNewlines));
        replace(wformat, phWtime, walltimeToString<StringT>(walltimeFormat, walltimeShift));
        replace(wformat, phTimestamp, toString<StringT>(std::to_string(timestamp())));
        replace(wformat, phApid, apidToString<StringT>());
        replace(wformat, phCtid, ctidToString<StringT>());
        replace(wformat, phEcu, ecuToString<StringT>());
        replace(wformat, phMode, modeToString<StringT>());
        replace(wformat, phType, typeToString<StringT>());
        replace(wformat, phSessId, toString<StringT>(std::to_string(sessionId())));
        replace(wformat, phNum, toString<StringT>(std::to_string(num())));
        return wformat;
    }
private:
    template <typename StringT>
    StringT toString(const std::string s)
    {
        return StringT(s.begin(), s.end());
    }
    template <typename StringT>
    void replace(StringT & where, StringT & what, StringT && with) {
        where.replace(where.find(what), what.size(), with);
    }
    template <typename StringT>
    StringT textidToString(const TextId & t) {
        int len = 0;
        for (int i = 0; i < 4; ++i)
            if (t.data[i] >= 'A' && t.data[i] <= 'Z')
                len++;
        return StringT(t.data, t.data+len);
    }
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
