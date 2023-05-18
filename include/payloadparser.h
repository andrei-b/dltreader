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

#ifndef PAYLOADPARSER_H
#define PAYLOADPARSER_H

#include <string>
#include <vector>

namespace DLTReader
{

/*
 * The following values are imported from dlt_protocol.h
 */
enum class PayloadValueType {
    None = 0,
    Int8 = 33,
    Int16 = 34,
    Int32 = 35,
    Int64 = 36,
    Int128 = 37,
    UInt8 = 65,
    UInt16 = 66,
    UInt32 = 67,
    UInt64 = 68,
    UInt128 = 69,
    Float = 128,
    ASCIIString = 512,
    UTF8String = 0x8200,
    Binary = 1024
};

struct PayloadValue {
    PayloadValueType type = PayloadValueType::None;
    int32_t i32val = 0;
    uint32_t ui32val = 0;
    uint64_t ui64val = 0;
    int64_t i64val = 0;
    std::string stringval;
    std::vector<char> binaryval;
    operator std::string() const
    {
        return stringval;
    }
    const char* data() const;
    uint16_t dataSize() const;
    operator uint16_t() const
    {
        return (uint16_t)ui32val;
    }
    operator int16_t() const
    {
        return (int16_t)i32val;
    }
    operator uint32_t() const
    {
        return ui32val;
    }
    operator int32_t() const
    {
        return i32val;
    }
    bool operator == (const std::string & other) const
    {
        return stringval == other;
    }
    bool operator == (const int32_t & other) const
    {
        return i32val == other;
    }
    bool operator == (const uint32_t & other) const
    {
        return ui32val == other;
    }
    template<typename T>
    bool operator != (const T & other) const
    {
        return !(*this == other);
    }
    bool operator > (uint32_t other) const
    {
        return ui32val > other;
    }
    bool operator >= (uint32_t other) const
    {
        return ui32val >= other;
    }
    bool operator < (uint32_t other) const
    {
        return !(*this >= other);
    }
};

class PayloadParser {
public:
    PayloadParser(const char * text, uint16_t len);
    PayloadValue readValue();
    std::string payloadAsString();
private:
    const char * mText;
    uint16_t length;
    uint16_t pos = 0;
};

}
#endif // PAYLOADPARSER_H
