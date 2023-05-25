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
#include "dltrecordcollection.h"
#include "dltfileparser.h"
#include "transferredfiles.h"
#include "payloadparser.h"
#include "version.h"
#include <textidfilter.h>
#include <iostream>
#include <stdio.h>


using namespace std;

int main()
{
    DLTReader::TextId t1 = "HMI ";
    DLTReader::TextIdSet set;
    set.push_back("HMI");
    DLTReader::TextIdFilter<DLTReader::TextIDField::ApId> filter(false, true, set);
    DLTReader::DirectDLTFile f1("/home/andrei/Downloads/test1.dlt");
    DLTReader::ParsedDLTRecord pr;
    for(auto r : f1) {
        if (filter.match(r)) {
        DLTReader::PayloadParser pp(r.payload,r.payloadLength);
        DLTReader::ParsedDLTRecord pr(r);
         std::cout << pr.recordToString<std::string>("{num}\t{ecu}\t{apid}\t{ctid}\t{sessid}\t{type}\t{mode}\t{walltime}\t{timestamp}\t{payload}", DLTReader::ParsedDLTRecord::DefaultTimeFormat) << std::endl;
        }

        if (r.num > 100000)
            break;
    }
    std::cout << pr.num() << "  " << pr.offset() << "  " << pr.rawDataAsString() << std::endl;
    return 0;
  /*      {
        DLTReader::DirectDLTFile f1("/home/andrei/Downloads/joinf.dlt");
        auto current = f1.begin();
        for (int i = 0; i < 32; i++) {
            std::cout << (*current).num << "  " << (*current).offset << "  " << (*current).length << "  " << (*current).parse().asString() << std::endl;
            ++current;
        }
    }
       DLTReader::DirectDLTFile f("/home/andrei/Downloads/joinf.dlt");
        //f1.reset();
        DLTReader::DLTRecordCollection collection(f);
        auto current1 = collection.begin();
        for (int i = 0; i < 32; i++) {
            auto s = (*current1).parse().asString();
            std::cout << (*current1).num << "  " << (*current1).offset << "  " << (*current1).length << "  " << (*current1).parse().asString() << std::endl;
            ++current1;
        }*/
    DLTReader::DirectDLTFile f("/home/andrei/Downloads/joinf.dlt");
     //f1.reset();
    DLTReader::DLTRecordCollection collection(f);
    DLTReader::TransferredFiles<DLTReader::DLTIndexedRecordIterator> files(collection.begin(), collection.end());
        while (files.findFile()) {
            printf("%s %i %s\n", files.currentFileName().data(), files.currentFileSize(), files.currentFileDate().data());
            auto contents = files.getCurrentFileContents();
            printf("%s\n", contents.data());
        }

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
