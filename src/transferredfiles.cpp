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

#include "transferredfiles.h"
#include "dltrecordparser.h"
#include <algorithm>

namespace DLTFile {

namespace Package {

    enum class ValueType {
        None = 0,
        String = 512,
        Int32 = 67
    };

    struct Value {
        ValueType type = ValueType::None;
        uint32_t i32val = 0;
        std::string stringval;
    };
    class PackageParser {
    public:
        PackageParser(const char * text, uint16_t len) : mText(text), mLength(len)
        {
            std::vector<char> v;
            for(int i = 0; i < len; ++i)  //REMOVE
                v.push_back(mText[i]);
            printf("%s", &v[0]);
        }
        Value readValue()
        {
            Value result;
            result.type = (ValueType)*((uint32_t *) (mText + pos));
            pos += 4;
            if (result.type == ValueType::String) {
                uint16_t valueLen = *((uint16_t *)(mText + pos));
                pos += 2;
                if (valueLen != 0) {
                    for(int i = pos; i < pos + valueLen - 1; ++i) // chopping off terminating 0
                        result.stringval += mText[i];
                    pos += valueLen;
                }
            }
            if (result.type == ValueType::Int32) {
                result.i32val = *((uint32_t*)&mText[pos]);
                pos += 4;
            }

            return result;
        }
    private:
        const char * mText;
        uint16_t mLength;
        uint16_t pos = 0;
    };
}

TransferredFiles::TransferredFiles(DLTFileRecordIterator begin, DLTFileRecordIterator end) : current(begin), end(end)
{

}

bool TransferredFiles::findFile()
{
    DLTRecordParser p;
    char flst[5] = {'F','L','S','T', '\0'};
    std::vector<char> Flst = {'F','L','S','T', '\0'};
    uint32_t * ptr = (uint32_t *)flst;
    while(current != end) {
        p.parseHeaders(*current);
        //auto r = p.extractRecord();
        if (p.payloadLength() > 10) {
            uint32_t * ptr2 = (uint32_t *)(p.payloadPointer()+6);
            if (*ptr == *ptr2) {
                Package::PackageParser pp(p.payloadPointer(), p.payloadLength());
                auto tag = pp.readValue();
                if (tag.stringval != "FLST") {
                    ++current;
                    continue;
                }
                auto id = pp.readValue();
                auto name = pp.readValue();
                fileName = name.stringval;
                auto size = pp.readValue();
                auto date = pp.readValue();
                auto blocks = pp.readValue();
                auto bsize = pp.readValue();
                tag  = pp.readValue();
                if (tag.stringval != "FLST") {
                    ++current;
                    continue;
                }
                ++current;
                return true;
            }
        }
        ++current;
    }
    return false;
}

std::string TransferredFiles::currentFileName() const
{
    return fileName;
}

}
