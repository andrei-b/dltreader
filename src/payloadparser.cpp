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

namespace DLTReader {

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
        if (result.type == PayloadValueType::ASCIIString || result.type == PayloadValueType::Binary) {
            uint16_t valueLen = *((uint16_t *)(mText + pos));
            pos += 2;
            if (valueLen != 0) {
                if (result.type == PayloadValueType::ASCIIString) {
                    for(int i = pos; i < pos + valueLen - 1; ++i) // chopping off terminating 0
                        result.stringval += mText[i];
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

std::string PayloadParser::payloadAsString()
{
    if (length == 0)
        return "NULL";
    std::string result;
    while (pos < length) {
    auto val = readValue();
        switch(val.type) {
        case PayloadValueType::ASCIIString:
            result = result + (result.size() > 0 ? " " : "") + val.stringval;
            break;
        case PayloadValueType::UTF8String:
            result = result + (result.size() > 0 ? " " : "") + val.stringval;
            break;
        case PayloadValueType::Int8:
        case PayloadValueType::Int16:
        case PayloadValueType::Int32:
            result = result + (result.size() > 0 ? " " : "") + std::to_string(val.i32val);
            break;
        case PayloadValueType::UInt8:
        case PayloadValueType::UInt16:
        case PayloadValueType::UInt32:
            result = result + (result.size() > 0 ? " " : "") + std::to_string(val.ui32val);
            break;
        case PayloadValueType::UInt64:
            result = result + (result.size() > 0 ? " " : "") + std::to_string(val.ui64val);
            break;
        case PayloadValueType::Int64:
            result = result + (result.size() > 0 ? " " : "") + std::to_string(val.i64val);
            break;
        case PayloadValueType::Binary:
            result = result + (result.size() > 0 ? " " : "") + "[Binary Data]";
            break;
        default:
            //auto ass = std::string(mText, length);
            result = result + (result.size() > 0 ? " " : "") + "[UNKNOWN]";
        }
    }
    return result;
}



const char *PayloadValue::data() const {
    switch (type) {
    case PayloadValueType::Binary:
        return binaryval.data();
    case PayloadValueType::ASCIIString:
        return stringval.data();
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
