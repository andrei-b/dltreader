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

#include "parsedrecordscollection.h"
#include "cacheddltfile.h"
#include "directdltfile.h"
#include "indexer.h"
#include "dltrecordparser.h"
#include "transferredfiles.h"
#include <iostream>
#include <stdio.h>


using namespace std;

int main()
{
        DLTFile::CachedDLTFile f("/home/andrei/Downloads/joinf.dlt");
        /*if (!f.init()) {
            printf("Init failed\n");
            return 0;
        }*/
        DLTFile::Indexer idx(f);
        auto index = idx.makeIndex();
        printf ("index: %i\n", index.size());
        f.reset();
        DLTFile::ParsedRecordsCollection records(f);
        DLTFile::TransferredFiles files(records.begin(), records.end());
        while (files.findFile())
            printf("FOUND\n");

      /*  for(const DLTFile::DLTFileRecordParsed & record : records) {
            //printf("%i:%s\n", record.num, record.ecu.data);
            char flst[4] = {'F','L','S','T'};
            if (record.payloadPtr.size() > 10) {
            uint32_t * ptr = (uint32_t *)&record.payloadPtr.data()[6];
            uint32_t * ptr2 = (uint32_t *)flst;
            //if (std::string(record.payloadPtr->begin(), record.payloadPtr->end()).find("FLST")!=std::string::npos) {
            if (*ptr == *ptr2) {
                    if (record.ecu == "FLST")
                    printf("ecu =FLST");
                if (record.apid == "FLST")
                    printf("apid =FLST");
                if (record.ctid == "FLST")
                    printf("ctid =FLST");
                if (std::string(record.payloadPtr.begin(), record.payloadPtr.end()).find("FLST")!=std::string::npos)
                    printf("Payload = %s", record.payloadPtr.data());
                printf("%i:%s\n", record.num, "FLST");
//                break;
            }
            }

        }*/
        //printf ("index: %i\n", index2.size());
        return 0;
}
