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

#include "indexer.h"

namespace DLTFile {

SparceIndex merge(const SparceIndex &index1, const SparceIndex &index2)
{
    SparceIndex result;
    result.fileName = index1.fileName;
    uint32_t i1 = 0, i2 = 0;
    while (i1 < index1.records.size() && i2 < index2.records.size()) {
        if (index1.records.at(i1).offset < index2.records.at(i2).offset) {
            result.records.push_back(index1.records.at(i1));
            ++i1;
        } else {
            if (index1.records.at(i1).offset > index2.records.at(i2).offset) {
                result.records.push_back(index2.records.at(i2));
                ++i2;
            }  else {
                result.records.push_back(index2.records.at(i2));
                ++i1;
                ++i2;
            }
        }
        while (i1 < index1.records.size()) {
            result.records.push_back(index1.records.at(i1));
            ++i1;
        }
        while (i2 < index2.records.size()) {
            result.records.push_back(index2.records.at(i2));
            ++i2;
        }
        return result;
    }

}

}
