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

#include "dltfilerecord.h"
#include "dltrecordparser.h"

namespace DLTReader {

bool DLTFileRecord::operator ==(const DLTFileRecord &other) const
{
    return this->num == other.num && this->offset == other.offset && this->good == other.good && this->length == other.length && this->msg == other.msg;
}

void DLTFileRecord::lightParse()
{
    DLTRecordParser p;
    if (!p.parseHeaders(*this)) {
        apid = nullptr;
        ctid = nullptr;
        ecu = nullptr;
        payload = nullptr;
        payloadLength = 0;
        return;
    }
    apid = p.apid();
    ctid = p.ctid();
    ecu = p.ecuPtr();
    payload = p.payloadPointer();
    payloadLength = p.payloadLength();
    headerParsed = true;
}

}
