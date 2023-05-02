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

#include "dltrecordparser.h"



namespace DLTReader {




DLTRecordParser::DLTRecordParser()
{
}

DLTRecordParser::~DLTRecordParser()
{
}

const uint16_t StorageHeaderSize = sizeof(DltStorageHeader);
const uint16_t StandardHeaderSize = sizeof(DltStandardHeader);
const uint16_t ExtendedHeaderSize = sizeof(DltExtendedHeader);

bool DLTRecordParser::parseHeaders(const DLTFileRecord &record)
{
    if (record.msg == nullptr || record.length == 0)
        return false;
    messageNumber = record.num;
    offset = record.offset;
    good = record.good;
    if(record.length < StorageHeaderSize+StandardHeaderSize) {
        good = false;
        return false;
    }

    storageHeader = (DltStorageHeader*) record.msg;
    standardHeader = (DltStandardHeader*)(((char*)record.msg) + StorageHeaderSize);
    length_t extra_size = DLT_STANDARD_HEADER_EXTRA_SIZE(standardHeader->htyp)+(DLT_IS_HTYP_UEH(standardHeader->htyp) ? ExtendedHeaderSize : 0);
    headerSize = StorageHeaderSize + StandardHeaderSize + extra_size;
    if(DLT_SWAP_16(standardHeader->len)<(static_cast<int>(headerSize) - StorageHeaderSize))
        payloadSize = 0;
    else
        payloadSize =  DLT_SWAP_16(standardHeader->len) - (headerSize - StorageHeaderSize);
    //if (payloadPtr == nullptr)
    //    payloadPtr.reset(new std::vector<char>());
    //else
    if (record.length  < (int)(headerSize)) {
        good = false;
        return false;
    }
    if (extra_size > 0)  {
        if (DLT_IS_HTYP_UEH(standardHeader->htyp)) {
            extendedHeader = (DltExtendedHeader*) (((char*)record.msg) + StorageHeaderSize + StandardHeaderSize +
                                  DLT_STANDARD_HEADER_EXTRA_SIZE(standardHeader->htyp));
        } else
            extendedHeader = nullptr;
        if (DLT_IS_HTYP_WEID(standardHeader->htyp)) {
            char * ptr = record.msg + StorageHeaderSize + StandardHeaderSize;
            headerExtra.ecu[0] = ptr[0];
            headerExtra.ecu[1] = ptr[1];
            headerExtra.ecu[2] = ptr[2];
            headerExtra.ecu[3] = ptr[3];

            uint32_t * iptr = (uint32_t *)(record.msg + StorageHeaderSize + StandardHeaderSize
                              + (DLT_IS_HTYP_WEID(standardHeader->htyp) ? DLT_SIZE_WEID : 0));
            headerExtra.seid = DLT_BETOH_32(*iptr);

            iptr = (uint32_t *)(record.msg + StorageHeaderSize + StandardHeaderSize
                    + (DLT_IS_HTYP_WEID(standardHeader->htyp) ? DLT_SIZE_WEID : 0)
                    + (DLT_IS_HTYP_WSID(standardHeader->htyp) ? DLT_SIZE_WSID : 0));
            headerExtra.tmsp = DLT_BETOH_32(*iptr);
        }
    }
    if (record.length  < (int)(headerSize+payloadSize)) {
        good = false;
        return false;
    }
    payloadPtr = record.msg + headerSize;
    return true;
}

char *DLTRecordParser::payloadPointer()
{
    return payloadPtr;
}

uint16_t DLTRecordParser::payloadLength()
{
    if (!good)
        return 0;
    return payloadSize;
}

TextId DLTRecordParser::ecu() const
{
    return headerExtra.ecu;
}

const char *DLTRecordParser::ecuPtr() const
{
    return headerExtra.ecu;
}

const char *DLTRecordParser::apid() const
{
    if (DLT_IS_HTYP_UEH(standardHeader->htyp))
        if (extendedHeader->apid[0] != 0)
            return extendedHeader->apid;
    return nullptr;
}

const char *DLTRecordParser::ctid() const
{
    if (DLT_IS_HTYP_UEH(standardHeader->htyp))
        if (extendedHeader->ctid[0] != 0)
            return extendedHeader->ctid;
    return nullptr;
}

ParsedDLTRecord DLTRecordParser::extractRecord()
{
    ParsedDLTRecord out;
    out.num = messageNumber;
    out.offset = offset;
    out.good = good;
    if(storageHeader) {
        out.seconds = storageHeader->seconds;
        out.microseconds = storageHeader->microseconds;
    } else {
        out.seconds = 0;
        out.microseconds = 0;
    }
    if ( DLT_IS_HTYP_WTMS(standardHeader->htyp) )
        out.timestamp = headerExtra.tmsp;
     else
        out.timestamp = 0;

    if (DLT_IS_HTYP_WEID(standardHeader->htyp))
        out.ecu = headerExtra.ecu;
    else {
        if(storageHeader)
            out.ecu = storageHeader->ecu;
    }

    if (DLT_IS_HTYP_UEH(standardHeader->htyp)) {
        if (extendedHeader->apid[0] != 0)
            out.apid = extendedHeader->apid;
        if (extendedHeader->ctid[0]!=0)
           out.ctid = extendedHeader->ctid;
        out.type = (DLTMessageType) DLT_GET_MSIN_MSTP(extendedHeader->msin);
        out.subtype = DLT_GET_MSIN_MTIN(extendedHeader->msin);
        if(DLT_IS_MSIN_VERB(extendedHeader->msin)) {
            out.mode = DLTLogMode::Verbose;
        } else {
            out.mode = out.mode = DLTLogMode::NonVerbose;
        }
    } else
        out.mode = DLTLogMode::NonVerbose;
    out.payloadSize = payloadSize;
    out.set(payloadPtr, payloadSize);
    out.sessionId = headerExtra.seid;
    return out;
}



}
