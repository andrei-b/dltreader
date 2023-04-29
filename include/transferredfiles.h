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

#ifndef TRANSFERREDFILES_H

#define TRANSFERREDFILES_H

#include "dltrecordparser.h"
#include <string>
#include <vector>

namespace DLTReader {

enum class FileTransferError {
    NoError = 0,
    BlockMismatch,
    WrongSize,
    ServerError,
    WrongDLTFormat
};

class TransferredFiles
{
public:
    TransferredFiles(DLTFileRecordIterator begin, DLTFileRecordIterator end);
    bool findFile();
    std::string currentFileName() const;
    std::string currentFileDate() const;
    uint32_t currentFileSize() const;
    uint32_t currentFileId() const;
    bool saveCurrentFile(const std::string & filename);
    FileTransferError errorInCurrentFile();
    bool endOfCurrentFile();
     std::vector<char> getCurrentFileContents();
private:
    std::vector<char> readBlock();
    DLTFileRecordIterator current;
    DLTFileRecordIterator end;
    std::string fileName;
    uint32_t fileSize;
    uint32_t blocks;
    uint32_t bsize;
    uint32_t fileId;
    uint32_t bytesRead = 0;
    bool eof = false;
    std::string fileDate;
    FileTransferError error = FileTransferError::NoError;
};

}

#endif // TRANSFERREDFILES_H
