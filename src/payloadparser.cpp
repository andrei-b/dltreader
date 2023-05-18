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

#include "payloadparser.h"
#include <locale>
#include <codecvt>

namespace DLTReader {

template <typename T>
void fromUTF8(const std::string& source, std::basic_string<T, std::char_traits<T>, std::allocator<T>>& result)
{
    std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
    result = convertor.from_bytes(source);
}

PayloadParser::PayloadParser(const char *text, uint16_t len) : mText(text), length(len)
{
    if (mText == nullptr)
        length = 0;
}

PayloadValue PayloadParser::readValue()
{
    PayloadValue result;
    if (pos < length) {
        result.type = (PayloadValueType)*((uint32_t *) (mText + pos));
        pos += 4;
        if (result.type == PayloadValueType::ASCIIString || result.type == PayloadValueType::Binary || result.type == PayloadValueType::UTF8String) {
            uint16_t valueLen = *((uint16_t *)(mText + pos));
            pos += 2;
            if (valueLen != 0) {
                if (result.type == PayloadValueType::ASCIIString) {
                        auto tmp = std::string(&mText[pos], &mText[pos + valueLen-1]);
                        result.stringval = std::u32string(tmp.begin(), tmp.end());
                } else
                    if (result.type == PayloadValueType::UTF8String) {
                        fromUTF8(std::string(&mText[pos], &mText[pos + valueLen-1]), result.stringval);
                    } else {
                                    result.binaryval.assign(mText + pos, mText + pos + valueLen);
                    }
                pos += valueLen;
            }
        } else
            if (result.type == PayloadValueType::UInt32 || result.type == PayloadValueType::Int32) {
                if (result.type == PayloadValueType::UInt32)
                    result.ui32val = *((uint32_t*)&mText[pos]);
                else
                    result.i32val = *((int32_t*)&mText[pos]);
                pos += 4;
            }
        if (result.type == PayloadValueType::UInt16 || result.type == PayloadValueType::Int16) {
            if (result.type == PayloadValueType::UInt16)
                result.ui32val = *((uint16_t*)&mText[pos]);
            else
                result.i32val = *((int16_t*)&mText[pos]);
            pos += 2;
        }
        if (result.type == PayloadValueType::UInt64 || result.type == PayloadValueType::Int64) {
            if (result.type == PayloadValueType::UInt64)
                result.ui64val = *((uint64_t*)&mText[pos]);
            else
                result.i64val = *((int64_t*)&mText[pos]);
            pos += 8;
        }

    }
    return result;
}

std::u32string PayloadParser::payloadAsU32String()
{
    if (length == 0)
        return U"NULL";
    std::u32string result;
    while (pos < length) {
        auto val = readValue();
        std::string strval;
        switch(val.type) {
        case PayloadValueType::ASCIIString:
        case PayloadValueType::UTF8String:
            result = result + (result.size() > 0 ? U" " : U"") + val.stringval;
            break;
        case PayloadValueType::Int8:
        case PayloadValueType::Int16:
        case PayloadValueType::Int32:
            strval = std::to_string(val.i32val);
            result = result + (result.size() > 0 ? U" " : U"") + std::u32string(strval.begin(), strval.end());
            break;
        case PayloadValueType::UInt8:
        case PayloadValueType::UInt16:
        case PayloadValueType::UInt32:
            strval = std::to_string(val.ui32val);
            result = result + (result.size() > 0 ? U" " : U"") + std::u32string(strval.begin(), strval.end());
            break;
        case PayloadValueType::UInt64:
            strval = std::to_string(val.ui64val);
            result = result + (result.size() > 0 ? U" " : U"") + std::u32string(strval.begin(), strval.end());
            break;
        case PayloadValueType::Int64:
            strval = std::to_string(val.i64val);
            result = result + (result.size() > 0 ? U" " : U"") + std::u32string(strval.begin(), strval.end());
            break;
        case PayloadValueType::Binary:
            result = result + (result.size() > 0 ? U" " : U"") + U"[Binary Data]";
            break;
        default:
            //auto ass = std::string(mText, length);
            result = result + (result.size() > 0 ? U" " : U"") + U"[UNKNOWN]";
        }
    }
    return result;
}

std::string PayloadParser::payloadAsString()
{
    auto s32 = payloadAsU32String();
    return std::string(s32.begin(), s32.end());
}



const char *PayloadValue::data() const {
    switch (type) {
    case PayloadValueType::Binary:
        return binaryval.data();
    case PayloadValueType::ASCIIString:
    case PayloadValueType::UTF8String:
        return (char *)stringval.data();
    default:
        return nullptr;
    }
}

uint16_t PayloadValue::dataSize() const {
    switch (type) {
    case PayloadValueType::Binary:
        return binaryval.size();
    case PayloadValueType::ASCIIString:
        return stringval.size();
    default:
        return 0;
    }
}

}
