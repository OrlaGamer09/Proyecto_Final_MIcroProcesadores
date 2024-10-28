///////////////////////////////////////////////////////////////////////////////
//
// ZX-ESPectrum - ZX Spectrum emulator for ESP32
//
// Copyright (c) 2020, 2021 David Crespo [dcrespo3d]
// https://github.com/dcrespo3d/ZX-ESPectrum-Wiimote
//
// Based on previous work by Ramón Martinez, Jorge Fuertes and many others
// https://github.com/rampa069/ZX-ESPectrum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef Config_h
#define Config_h

#include <Arduino.h>

class Config
{
public:
    // machine type change request
    static void requestMachine(String newArch, String newRomSet, bool force);

    // config variables
    static const String& getArch()   { return arch;   }
    static const String& getRomSet() { return romSet; }
    static String   ram_file;
    static bool     slog_on;
    static bool     aspect_16_9;

    // config persistence
    static void           load();
    static void           save();

    // list of snapshot file names
    static String   sna_file_list;
    // list of snapshot display names
    static String   sna_name_list;
    // load lists of snapshots
    static void loadSnapshotLists();

    // list of TAP file names
    static String   tap_file_list;
    // list of TAP display names
    static String   tap_name_list;
    // load lists of TAP files
    static void loadTapLists();

private:
    static String   arch;
    static String   romSet;
};

#endif // Config.h