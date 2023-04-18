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
#include "parsedrecordscollection.h"
#include <string>

namespace DLTFile {

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
private:
    DLTFileRecordIterator current;
    DLTFileRecordIterator end;
    std::string fileName;
    uint32_t fileSize;
    uint32_t blocks;
    uint32_t bsize;
    uint32_t fileId;
    std::string fileDate;
};

}

#endif // TRANSFERREDFILES_H
