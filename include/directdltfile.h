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

#ifndef DIRECTDLTFILE_H
#define DIRECTDLTFILE_H

#include "dltfileparser.h"
#include <vector>
#include <string>
#include<iostream>
#include<fstream>

namespace DLTFile {

class DirectDLTFile : public DLTFileParser
{
    static const uint64_t BufferSize = 4096*1024; // this should be greater than the maximum size of the dlt message (64k).
public:
    explicit DirectDLTFile(const std::string & fileName);
    virtual ~DirectDLTFile();
protected:
    bool resetFile() override;
    virtual uint64_t readFile(BufPtr &buf) override;
    void seek(uint64_t pos) override;
    void closeFile() override;
private:
    std::ifstream mFile;
    std::vector<char> buffer;
};

}

#endif // DIRECTDLTFILE_H
