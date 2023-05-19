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

#ifndef VERSION_H
#define VERSION_H

namespace DLTReader {

#define API_VERSION "0.1b"
constexpr char APIVersion[] = API_VERSION;

#define PACKAGE_VERSION "0.1b"
constexpr char PackageVersion[] = PACKAGE_VERSION;

#define PACKAGE_STATE "unstable"
constexpr char PackageState[] = PACKAGE_STATE;

}

#endif // VERSION_H
