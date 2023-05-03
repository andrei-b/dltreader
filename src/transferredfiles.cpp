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
#include <iostream>
#include <fstream>

namespace DLTReader {

namespace Package {

/*
 * The following values are imported from dlt_protocol.h
 */
    enum class ValueType {
        None = 0,
        Int8 = 33,
        Int16 = 34,
        Int32 = 35,
        Int64 = 36,
        Int128 = 37,
        UInt8 = 65,
        UInt16 = 66,
        UInt32 = 67,
        Uint64 = 68,
        Uint128 = 69,
        Float = 128,
        ASCIIString = 512,
        UTF8String = 0x8200,
        Binary = 1024
    };

    struct Value {
        ValueType type = ValueType::None;
        int32_t i32val = 0;
        uint32_t ui32val = 0;
        std::string stringval;
        std::vector<char> binaryval;
        operator std::string() const
        {
            return stringval;
        }
        const char* data() const {
            switch (type) {
            case ValueType::Binary:
                return binaryval.data();
            case ValueType::ASCIIString:
                return stringval.data();
            default:
                return nullptr;
            }
        }
        uint16_t dataSize() const {
            switch (type) {
            case ValueType::Binary:
                return binaryval.size();
            case ValueType::ASCIIString:
                return stringval.size();
            default:
                return 0;
            }
        }
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

    class PackageParser {
    public:
        PackageParser(const char * text, uint16_t len) : mText(text), length(len)
        {
        }
        Value readValue()
        {
            Value result;
            if (pos < length) {
                result.type = (ValueType)*((uint32_t *) (mText + pos));
                pos += 4;
                if (result.type == ValueType::ASCIIString || result.type == ValueType::Binary) {
                    uint16_t valueLen = *((uint16_t *)(mText + pos));
                    pos += 2;
                    if (valueLen != 0) {
                        if (result.type == ValueType::ASCIIString) {
                            for(int i = pos; i < pos + valueLen - 1; ++i) // chopping off terminating 0
                                result.stringval += mText[i];
                        } else {
                            result.binaryval.assign(mText + pos, mText + pos + valueLen);
                        }
                        pos += valueLen;
                    }
                } else
                if (result.type == ValueType::UInt32 || result.type == ValueType::Int32) {
                    if (result.type == ValueType::UInt32)
                        result.ui32val = *((uint32_t*)&mText[pos]);
                    else
                        result.i32val = *((int32_t*)&mText[pos]);
                    pos += 4;
                }
                if (result.type == ValueType::UInt16 || result.type == ValueType::Int16) {
                    if (result.type == ValueType::UInt16)
                        result.ui32val = *((uint16_t*)&mText[pos]);
                    else
                        result.i32val = *((int16_t*)&mText[pos]);
                    pos += 2;
                }
            }
            return result;
        }
    private:
        const char * mText;
        uint16_t length;
        uint16_t pos = 0;
    };
}


template<typename Iterator>
TransferredFiles<Iterator>::TransferredFiles(Iterator begin, Iterator end) :  current(begin), end(end)
{

}

template<typename Iterator>
bool TransferredFiles<Iterator>::findFile()
{
    DLTRecordParser p;
    char flst[5] = {'F','L','S','T', '\0'};
    uint32_t * ptr = (uint32_t *)flst;
    while(current != end) {
        p.parseHeaders(*current);
        if (p.payloadLength() > 10) {
            uint32_t * ptr2 = (uint32_t *)(p.payloadPointer()+6);
            if (*ptr == *ptr2) {
                Package::PackageParser pp(p.payloadPointer(), p.payloadLength());
                auto tag = pp.readValue();
                if (tag != "FLST") {
                    ++current;
                    continue;
                }
                auto id = pp.readValue();
                fileId = id;
                auto name = pp.readValue();
                fileName = (std::string)name;
                auto size = pp.readValue();
                fileSize = size;
                auto date = pp.readValue();
                fileDate = (std::string)date;
                auto blocks = pp.readValue();
                this->blocks = blocks;
                auto bsize = pp.readValue();
                this->bsize = bsize;
                tag  = pp.readValue();
                if (tag != "FLST") {
                    ++current;
                    continue;
                }
                ++current;
                error = FileTransferError::NoError;
                bytesRead = 0;
                eof = false;
                return true;
            }
        }
        ++current;
    }
    return false;
}

template<typename Iterator>
std::string TransferredFiles<Iterator>::currentFileName() const
{
    return fileName;
}

template<typename Iterator>
std::string TransferredFiles<Iterator>::currentFileDate() const
{
    return fileDate;
}

template<typename Iterator>
uint32_t TransferredFiles<Iterator>::currentFileSize() const
{
    return fileSize;
}

template<typename Iterator>
uint32_t TransferredFiles<Iterator>::currentFileId() const
{
    return fileId;
}

template<typename Iterator>
bool TransferredFiles<Iterator>::saveCurrentFile(const std::string &filename)
{
    std::ofstream file;
    file.open(filename);
    if (!file.is_open())
        return false;
    auto block = readBlock();
    while (block.size() != 0) {
        file.write(block.data(), block.size());
        block = readBlock();
    }
    file.close();
    if (error != FileTransferError::NoError)
        return false;
    return true;
}

template<typename Iterator>
FileTransferError TransferredFiles<Iterator>::errorInCurrentFile()
{
    return error;
}

template<typename Iterator>
bool TransferredFiles<Iterator>::endOfCurrentFile()
{
    return eof;
}

template<typename Iterator>
std::vector<char> TransferredFiles<Iterator>::readBlock()
{
    if (eof || error != FileTransferError::NoError)
        return std::vector<char>();
    DLTRecordParser p;
    char flda[5] = {'F','L','D','A', '\0'};
    char flfi[5] = {'F','L','F','I', '\0'};
    char fler[5] = {'F','L','E','R', '\0'};
    uint32_t * fldaptr = (uint32_t *)flda;
    uint32_t * flfiptr = (uint32_t *)flfi;
    uint32_t * flerptr = (uint32_t *)fler;
    while(current != end) {
        p.parseHeaders(*current);
        //auto r = p.extractRecord();
        if (p.payloadLength() > 10) {
            uint32_t * ptr = (uint32_t *)(p.payloadPointer()+6);
            Package::PackageParser pp(p.payloadPointer(), p.payloadLength());
            if (*ptr == *fldaptr) {
                auto tag = pp.readValue();
                if (tag != "FLDA") {
                    ++current;
                    continue;
                }
                auto id = pp.readValue();
                if (id != fileId)  {
                    ++current;
                    continue;
                }
                auto pkgNum = pp.readValue();
                if (pkgNum > blocks)
                    error = FileTransferError::BlockMismatch;
                auto payload = pp.readValue();
                tag  = pp.readValue();
                if (tag != "FLDA") {
                    error = FileTransferError::WrongDLTFormat;
                }
                ++current;
                return payload.binaryval;
            } else {
                if (*ptr == *flfiptr) {
                    pp.readValue();
                    auto id = pp.readValue();
                    if (id != fileId)  {
                        ++current;
                        continue;
                    }
                    eof = true;
                    if (bytesRead != fileSize)
                        error = FileTransferError::WrongSize;
                    return std::vector<char>();
                } else {
                    if (*ptr == *flerptr) {
                        pp.readValue();
                        auto id = pp.readValue();
                        if (id != fileId)
                            error = FileTransferError::WrongDLTFormat;
                        error = FileTransferError::ServerError;
                        return std::vector<char>();
                    }
                }
            }
        }
        ++current;
    }
    error = FileTransferError::WrongDLTFormat;
    return std::vector<char>();
}

template<typename Iterator>
std::vector<char> TransferredFiles<Iterator>::getCurrentFileContents()
{
    std::vector<char> result;
    auto block = readBlock();
    while (block.size() != 0) {
        result.insert(result.end(), std::make_move_iterator(block.begin()), std::make_move_iterator(block.end()));
        block = readBlock();
    }
    return result;
}
}
