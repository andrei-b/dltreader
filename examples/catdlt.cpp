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
#include "directdltfile.h"
#include "parseddltrecord.h"
#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, char ** argv)
{
    if (argc == 1) {
        printf("This program dumps text information from given dlt file.\n");
        return 0;
    }
    DLTReader::DirectDLTFile dltFile(argv[1]);
    for(auto rawRecord : dltFile) {
        DLTReader::ParsedDLTRecord parsedRecord(rawRecord);
        std::cout << parsedRecord.recordToString<std::string>("{num}\t{ecu}\t{apid}\t{ctid}\t{sessid}\t{type}\t{mode}\t{walltime}\t{timestamp}\t{payload}", DLTReader::ParsedDLTRecord::DefaultTimeFormat) << std::endl;
    }
    return 0;
}
