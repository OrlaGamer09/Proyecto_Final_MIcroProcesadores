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

#ifndef ESPECTRUM_OSD_H
#define ESPECTRUM_OSD_H

// OSD Headers
#include <Arduino.h>
#include "hardconfig.h"

// Defines
#define OSD_FONT_W 6
#define OSD_FONT_H 8

#define LEVEL_INFO 0
#define LEVEL_OK 1
#define LEVEL_WARN 2
#define LEVEL_ERROR 3

// OSD Interface
class OSD
{
public:
    // ZX Color
    static uint16_t zxColor(uint8_t color, uint8_t bright);

    // Screen size to be set at initialization
    static unsigned short scrW;
    static unsigned short scrH;

    // Calc
    static unsigned short scrAlignCenterX(unsigned short pixel_width);
    static unsigned short scrAlignCenterY(unsigned short pixel_height);
    static byte osdMaxRows();
    static byte osdMaxCols();
    static unsigned short osdInsideX();
    static unsigned short osdInsideY();

    // OSD
    static void osdHome();
    static void osdAt(byte row, byte col);
    static void drawOSD();
    static void do_OSD();

    // Error
    static void errorPanel(String errormsg);
    static void errorHalt(String errormsg);
    static void osdCenteredMsg(String msg, byte warn_level);

    // Menu
    static void newMenu(String new_menu);
    static void menuRecalc();
    static unsigned short menuRealRowFor(byte virtual_row_num);
    static void menuPrintRow(byte virtual_row_num, byte line_type);
    static void menuDraw();
    static void menuRedraw();
    static String getArchMenu();
    static String getRomsetMenu(String arch);
    static unsigned short menuRun(String new_menu);
    static void menuScroll(boolean up);
    static void menuAt(short int row, short int col);
    static void menuScrollBar();
    static String getTestMenu(unsigned short n_lines);

    // Rows
    static unsigned short rowCount(String menu);
    static String rowGet(String menu, unsigned short row_number);

    // Snapshot (SNA/Z80) Management
    static void changeSnapshot(String sna_filename);
};

#endif // ESPECTRUM_OSD_H
