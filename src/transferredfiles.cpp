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
#include "payloadparser.h"
#include <algorithm>
#include <iostream>
#include <fstream>

namespace DLTReader {

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
                PayloadParser pp(p.payloadPointer(), p.payloadLength());
                auto tag = pp.readValue();
                if ((std::string)tag != "FLST") {
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
                if ((std::string)tag != "FLST") {
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
            PayloadParser pp(p.payloadPointer(), p.payloadLength());
            if (*ptr == *fldaptr) {
                auto tag = pp.readValue();
                if ((std::string)tag != "FLDA") {
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
                if ((std::string)tag != "FLDA") {
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
