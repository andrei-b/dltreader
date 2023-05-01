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

#include "dltrecordcollection.h"

namespace DLTReader {


DLTRecordCollection::DLTRecordCollection(DLTFileParser &source, const DLTFilterBase &filter) : fileName(source.fileName())
{
}

DLTRecordCollection::DLTRecordCollection(DLTFileParser & source) : fileName(source.fileName())
{

}

DLTRecordCollection::DLTRecordCollection(DLTRecordCollection &source, const DLTFilterBase &filter) : fileName(source.fileName())
{

}

}
