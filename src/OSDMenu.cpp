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

#include "FileUtils.h"
#include "PS2Kbd.h"
#include "ESPectrum.h"
#include "messages.h"
#include "osd.h"
#include "Wiimote2Keys.h"
#include <math.h>

#define MENU_MAX_ROWS 23
// Line type
#define IS_TITLE 0
#define IS_FOCUSED 1
#define IS_NORMAL 2
// Scroll
#define UP true
#define DOWN false

extern Font Font6x8;

static byte cols;                     // Maximum columns
static unsigned short real_rows;      // Real row count
static byte virtual_rows;             // Virtual maximum rows on screen
static byte w;                        // Width in pixels
static byte h;                        // Height in pixels
static byte x;                        // X vertical position
static byte y;                        // Y horizontal position
static String menu;                   // Menu string
static unsigned short begin_row;      // First real displayed row
static byte focus;                    // Focused virtual row
static byte last_focus;               // To check for changes
static unsigned short last_begin_row; // To check for changes

#define NUM_SPECTRUM_COLORS 16

static word spectrum_colors[NUM_SPECTRUM_COLORS] = {
    BLACK,     BLUE,     RED,     MAGENTA,     GREEN,     CYAN,     YELLOW,     WHITE,
    BRI_BLACK, BRI_BLUE, BRI_RED, BRI_MAGENTA, BRI_GREEN, BRI_CYAN, BRI_YELLOW, BRI_WHITE,
};

uint16_t OSD::zxColor(uint8_t color, uint8_t bright) {
    if (bright) color += 8;
    return spectrum_colors[color];
}

// Set menu and force recalc
void OSD::newMenu(String new_menu) {
    menu = new_menu;
    menuRecalc();
    menuDraw();
}

void OSD::menuRecalc() {
    // Columns
    cols = 24;
    byte col_count = 0;
    for (unsigned short i = 0; i < menu.length(); i++) {
        if (menu.charAt(i) == ASCII_NL) {
            if (col_count > cols) {
                cols = col_count;
            }
            col_count = 0;
        }
        col_count++;
    }
    cols = (cols > osdMaxCols() ? osdMaxCols() : cols);

    // Rows
    real_rows = rowCount(menu);
    virtual_rows = (real_rows > MENU_MAX_ROWS ? MENU_MAX_ROWS : real_rows);
    begin_row = last_begin_row = last_focus = focus = 1;

    // Size
    w = (cols * OSD_FONT_W) + 2;
    h = (virtual_rows * OSD_FONT_H) + 2;

    // Position
    x = scrAlignCenterX(w);
    y = scrAlignCenterY(h);
}

// Get real row number for a virtual one
unsigned short OSD::menuRealRowFor(byte virtual_row_num) { return begin_row + virtual_row_num - 1; }

// Menu relative AT
void OSD::menuAt(short int row, short int col) {
    if (col < 0)
        col = cols - 2 - col;
    if (row < 0)
        row = virtual_rows - 2 - row;
    ESPectrum::vga.setCursor(x + 1 + (col * OSD_FONT_W), y + 1 + (row * OSD_FONT_H));
}

// Print a virtual row
void OSD::menuPrintRow(byte virtual_row_num, byte line_type) {
    VGA& vga = ESPectrum::vga;
    byte margin;
    String line = rowGet(menu, menuRealRowFor(virtual_row_num));

    switch (line_type) {
    case IS_TITLE:
        vga.setTextColor(OSD::zxColor(7, 0), OSD::zxColor(0, 0));
        margin = 2;
        break;
    case IS_FOCUSED:
        vga.setTextColor(OSD::zxColor(0, 1), OSD::zxColor(5, 1));
        margin = (real_rows > virtual_rows ? 3 : 2);
        break;
    default:
        vga.setTextColor(OSD::zxColor(0, 1), OSD::zxColor(7, 1));
        margin = (real_rows > virtual_rows ? 3 : 2);
    }

    menuAt(virtual_row_num, 0);
    vga.print(" ");
    if (line.length() < cols - margin) {
        vga.print(line.c_str());
        for (byte i = line.length(); i < (cols - margin); i++)
            vga.print(" ");
    } else {
        vga.print(line.substring(0, cols - margin).c_str());
    }
    vga.print(" ");
}

// Draw the complete menu
void OSD::menuDraw() {
    VGA& vga = ESPectrum::vga;
    // Set font
    vga.setFont(Font6x8);
    // Menu border
    vga.rect(x, y, w, h, OSD::zxColor(0, 0));
    // Title
    menuPrintRow(0, IS_TITLE);
    // Rainbow
    unsigned short rb_y = y + 8;
    unsigned short rb_paint_x = x + w - 30;
    byte rb_colors[] = {2, 6, 4, 5};
    for (byte c = 0; c < 4; c++) {
        for (byte i = 0; i < 5; i++) {
            vga.line(rb_paint_x + i, rb_y, rb_paint_x + 8 + i, rb_y - 8, OSD::zxColor(rb_colors[c], 1));
        }
        rb_paint_x += 5;
    }
    // Focused first line
    menuPrintRow(1, IS_FOCUSED);
    for (byte r = 2; r < virtual_rows; r++) {
        menuPrintRow(r, IS_NORMAL);
    }
    focus = 1;
    menuScrollBar();
}

String OSD::getArchMenu() {
    String menu = (String)MENU_ARCH + FileUtils::getFileEntriesFromDir(DISK_ROM_DIR);
    return menu;
}

String OSD::getRomsetMenu(String arch) {
    String menu = (String)MENU_ROMSET + FileUtils::getFileEntriesFromDir((String)DISK_ROM_DIR + "/" + arch);
    return menu;
}

// Run a new menu
unsigned short OSD::menuRun(String new_menu) {
    newMenu(new_menu);
    while (1) {
        updateWiimote2KeysOSD();
        if (PS2Keyboard::checkAndCleanKey(KEY_CURSOR_UP)) {
            if (focus == 1 and begin_row > 1) {
                menuScroll(DOWN);
            } else if (focus > 1) {
                focus--;
                menuPrintRow(focus, IS_FOCUSED);
                if (focus + 1 < virtual_rows) {
                    menuPrintRow(focus + 1, IS_NORMAL);
                }
            }
        } else if (PS2Keyboard::checkAndCleanKey(KEY_CURSOR_DOWN)) {
            if (focus == virtual_rows - 1) {
                menuScroll(UP);
            } else if (focus < virtual_rows - 1) {
                focus++;
                menuPrintRow(focus, IS_FOCUSED);
                if (focus - 1 > 0) {
                    menuPrintRow(focus - 1, IS_NORMAL);
                }
            }
        } else if (PS2Keyboard::checkAndCleanKey(KEY_PAGE_UP)) {
            if (begin_row > virtual_rows) {
                focus = 1;
                begin_row -= virtual_rows;
            } else {
                focus = 1;
                begin_row = 1;
            }
            menuRedraw();
        } else if (PS2Keyboard::checkAndCleanKey(KEY_PAGE_DOWN)) {
            if (real_rows - begin_row  - virtual_rows > virtual_rows) {
                focus = 1;
                begin_row += virtual_rows - 1;
            } else {
                focus = virtual_rows - 1;
                begin_row = real_rows - virtual_rows + 1;
            }
            menuRedraw();
        } else if (PS2Keyboard::checkAndCleanKey(KEY_HOME)) {
            focus = 1;
            begin_row = 1;
            menuRedraw();
        } else if (PS2Keyboard::checkAndCleanKey(KEY_END)) {
            focus = virtual_rows - 1;
            begin_row = real_rows - virtual_rows + 1;
            menuRedraw();
        } else if (PS2Keyboard::checkAndCleanKey(KEY_ENTER)) {
            return menuRealRowFor(focus);
        } else if (PS2Keyboard::checkAndCleanKey(KEY_ESC) || PS2Keyboard::checkAndCleanKey(KEY_F1)) {
            return 0;
        }
    }
}

// Scroll
void OSD::menuScroll(boolean dir) {
    if (dir == DOWN and begin_row > 1) {
        begin_row--;
    } else if (dir == UP and (begin_row + virtual_rows - 1) < real_rows) {
        begin_row++;
    } else {
        return;
    }
    menuRedraw();
}

// Redraw inside rows
void OSD::menuRedraw() {
    if (focus != last_focus or begin_row != last_begin_row) {
        for (byte row = 1; row < virtual_rows; row++) {
            if (row == focus) {
                menuPrintRow(row, IS_FOCUSED);
            } else {
                menuPrintRow(row, IS_NORMAL);
            }
        }
        menuScrollBar();
        last_focus = focus;
        last_begin_row = begin_row;
    }
}

// Draw menu scroll bar
void OSD::menuScrollBar() {
    VGA& vga = ESPectrum::vga;
    if (real_rows > virtual_rows) {
        // Top handle
        menuAt(1, -1);
        if (begin_row > 1) {
            vga.setTextColor(OSD::zxColor(7, 0), OSD::zxColor(0, 0));
            vga.print("+");
        } else {
            vga.setTextColor(OSD::zxColor(7, 0), OSD::zxColor(0, 0));
            vga.print("-");
        }

        // Complete bar
        unsigned short holder_x = x + (OSD_FONT_W * (cols - 1)) + 1;
        unsigned short holder_y = y + (OSD_FONT_H * 2);
        unsigned short holder_h = OSD_FONT_H * (virtual_rows - 3);
        unsigned short holder_w = OSD_FONT_W;
        vga.fillRect(holder_x, holder_y, holder_w, holder_h + 1, OSD::zxColor(7, 0));
        holder_y++;

        // Scroll bar
        unsigned short shown_pct = round(((float)virtual_rows / (float)real_rows) * 100.0);
        unsigned short begin_pct = round(((float)(begin_row - 1) / (float)real_rows) * 100.0);
        unsigned short bar_h = round(((float)holder_h / 100.0) * (float)shown_pct);
        unsigned short bar_y = round(((float)holder_h / 100.0) * (float)begin_pct);
        while ((bar_y + bar_h) >= holder_h) {
            bar_h--;
        }

        vga.fillRect(holder_x + 1, holder_y + bar_y, holder_w - 2, bar_h, OSD::zxColor(0, 0));

        // Bottom handle
        menuAt(-1, -1);
        if ((begin_row + virtual_rows - 1) < real_rows) {
            vga.setTextColor(OSD::zxColor(7, 0), OSD::zxColor(0, 0));
            vga.print("+");
        } else {
            vga.setTextColor(OSD::zxColor(7, 0), OSD::zxColor(0, 0));
            vga.print("-");
        }
    }
}

// Return a test menu
String OSD::getTestMenu(unsigned short n_lines) {
    String test_menu = "Test Menu\n";
    for (unsigned short line = 1; line <= n_lines; line += 2) {
        test_menu += "Option Line " + (String)line + "\n";
        test_menu += "1........10........20........30........40........50........60\n";
    }
    return test_menu;
}
