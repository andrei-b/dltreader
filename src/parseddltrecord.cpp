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

#include "parseddltrecord.h"
#include "payloadparser.h"

namespace DLTReader {

ParsedDLTRecord::ParsedDLTRecord() : mNum(0), mOffset(0), mGood(false)
{
}

void ParsedDLTRecord::setPayload(const char *src, uint16_t length)
{
    data.reset(new char[length]);
    mPayloadSize = length;
    std::copy(src, src+mPayloadSize, data.get());
}

std::string ParsedDLTRecord::rawDataAsString() {
    if (data != nullptr)
        return std::string(data.get(), data.get()+mPayloadSize);
    return "";
}

void ParsedDLTRecord::setNum(uint32_t newNum)
{
    mNum = newNum;
}

uint64_t ParsedDLTRecord::offset() const
{
    return mOffset;
}

void ParsedDLTRecord::setOffset(uint64_t newOffset)
{
    mOffset = newOffset;
}

bool ParsedDLTRecord::good() const
{
    return mGood;
}

void ParsedDLTRecord::setGood(bool newGood)
{
    mGood = newGood;
}

TextId ParsedDLTRecord::ecu()
{
    return mEcu;
}

void ParsedDLTRecord::setEcu(const TextId &newEcu)
{
    mEcu = newEcu;
}

TextId ParsedDLTRecord::apid()
{
    return mApid;
}

TextId ParsedDLTRecord::ctid()
{
    return mCtid;
}

void ParsedDLTRecord::setCtid(const TextId &newCtid)
{
    mCtid = newCtid;
}

DLTMessageType ParsedDLTRecord::type() const
{
    return mType;
}

void ParsedDLTRecord::setType(DLTMessageType newType)
{
    mType = newType;
}

uint8_t ParsedDLTRecord::subtype() const
{
    return mSubtype;
}

void ParsedDLTRecord::setSubtype(uint8_t newSubtype)
{
    mSubtype = newSubtype;
}

DLTLogMode ParsedDLTRecord::mode() const
{
    return mMode;
}

void ParsedDLTRecord::setMode(DLTLogMode newMode)
{
    mMode = newMode;
}

uint32_t ParsedDLTRecord::sessionId() const
{
    return mSessionId;
}

void ParsedDLTRecord::setSessionId(uint32_t newSessionId)
{
    mSessionId = newSessionId;
}

uint32_t ParsedDLTRecord::seconds() const
{
    return mSeconds;
}

void ParsedDLTRecord::setSeconds(uint32_t newSeconds)
{
    mSeconds = newSeconds;
}

uint32_t ParsedDLTRecord::microseconds() const
{
    return mMicroseconds;
}

void ParsedDLTRecord::setMicroseconds(uint32_t newMicroseconds)
{
    mMicroseconds = newMicroseconds;
}

uint32_t ParsedDLTRecord::timestamp() const
{
    return mTimestamp;
}

void ParsedDLTRecord::setTimestamp(uint32_t newTimestamp)
{
    mTimestamp = newTimestamp;
}

uint16_t ParsedDLTRecord::payloadSize() const
{
    return mPayloadSize;
}

void ParsedDLTRecord::setPayloadSize(uint16_t newPayloadSize)
{
    mPayloadSize = newPayloadSize;
}

void ParsedDLTRecord::setApid(const TextId &newApid)
{
    mApid = newApid;
}

uint32_t ParsedDLTRecord::num() const
{
    return mNum;
}

template<typename T>
T ParsedDLTRecord::payloadAs()
{
    if (parsedPayload.size() == 0) {
        PayloadParser pp(data.get(), mPayloadSize);
        parsedPayload = pp.payloadAsU32String();
    }
    return T(parsedPayload.begin(), parsedPayload.end());
}

}
