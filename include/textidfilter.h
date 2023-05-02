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

#ifndef TEXTIDFILTER_H
#define TEXTIDFILTER_H

#include "dltfilterbase.h"
#include "parseddltrecord.h"
#include <vector>

namespace DLTReader
{

 using TextIdSet = std::vector<TextId>;


class ApIdFilter: public DLTFilterBase
{
public:
    ApIdFilter(bool positive, TextIdSet && set);
    bool virtual match(DLTFileRecord &record) const override;
    bool virtual match(const DLTRecordSet &records) override;
private:
    bool positive = true;
    TextIdSet set;
};

class CtIdFilter: public DLTFilterBase
{
public:
    CtIdFilter(bool positive, TextIdSet && set);
    bool virtual match(DLTFileRecord &record) const override;
    bool virtual match(const DLTRecordSet & records) override;
private:
    bool positive = true;
    TextIdSet set;
};

class EcuFilter: public DLTFilterBase
{
public:
    EcuFilter(bool positive, TextIdSet && set);
    bool virtual match(DLTFileRecord &record) const override;
    bool virtual match(const DLTRecordSet &records) override;
private:
    bool positive = true;
    TextIdSet set;
};

}

#endif // TEXTIDFILTER_H
