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
    }
    return result;
}

std::string PayloadParser::readValueAsString()
{
    if (length == 0)
        return "NULL";
    auto val = readValue();
    if (val.type == PayloadValueType::ASCIIString)
        return (std::string)val;
    else
        return "[Non-text data]";
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
