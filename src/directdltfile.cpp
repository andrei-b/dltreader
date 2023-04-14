/*
    DLTReader Diagnostic Log and Trace reading library
    Copyright (C) 2023 Andrei Borovsky <andrey.borovsky@gmail.com>
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "directdltfile.h"
#include <algorithm>
#include <stdio.h>

DirectDLTFile::DirectDLTFile(const std::string &fileName) : DLTFileParser(fileName)
{
    buffer.reserve(BufferSize);
}

DirectDLTFile::~DirectDLTFile()
{
    closeFile();
}

bool DirectDLTFile::resetFile()
{
    if (!mFile.is_open())
        mFile.open(fileName());
    if (mFile.is_open()) {
        return true;
    }
    return false;
}

uint64_t DirectDLTFile::readFile(BufPtr &buf)
{
    mFile.read(buffer.data(), BufferSize);
    buf = buffer.data();
    return mFile.gcount();
}

void DirectDLTFile::seek(uint64_t pos)
{
    mFile.seekg(pos);
}

void DirectDLTFile::closeFile()
{
    mFile.close();
}

