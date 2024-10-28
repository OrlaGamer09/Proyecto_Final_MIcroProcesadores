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

#include "osd.h"
#include "FileUtils.h"
#include "PS2Kbd.h"
#include "CPU.h"
#include "ESPectrum.h"
#include "messages.h"
#include "Wiimote2Keys.h"
#include "Config.h"
#include "FileSNA.h"
#include "FileZ80.h"
#include "AySound.h"
#include "Mem.h"
#include "Tape.h"
#include "pwm_audio.h"

#define MENU_REDRAW true
#define MENU_UPDATE false
#define OSD_ERROR true
#define OSD_NORMAL false

#define OSD_W 248
#define OSD_H 152
#define OSD_MARGIN 4

extern Font Font6x8;

unsigned short OSD::scrW = 320;
unsigned short OSD::scrH = 240;

// X origin to center an element with pixel_width
unsigned short OSD::scrAlignCenterX(unsigned short pixel_width) { return (scrW / 2) - (pixel_width / 2); }

// Y origin to center an element with pixel_height
unsigned short OSD::scrAlignCenterY(unsigned short pixel_height) { return (scrH / 2) - (pixel_height / 2); }

byte OSD::osdMaxRows() { return (OSD_H - (OSD_MARGIN * 2)) / OSD_FONT_H; }
byte OSD::osdMaxCols() { return (OSD_W - (OSD_MARGIN * 2)) / OSD_FONT_W; }
unsigned short OSD::osdInsideX() { return scrAlignCenterX(OSD_W) + OSD_MARGIN; }
unsigned short OSD::osdInsideY() { return scrAlignCenterY(OSD_H) + OSD_MARGIN; }

// Cursor to OSD first row,col
void OSD::osdHome() { ESPectrum::vga.setCursor(osdInsideX(), osdInsideY()); }

// Cursor positioning
void OSD::osdAt(byte row, byte col) {
    if (row > osdMaxRows() - 1)
        row = 0;
    if (col > osdMaxCols() - 1)
        col = 0;
    unsigned short y = (row * OSD_FONT_H) + osdInsideY();
    unsigned short x = (col * OSD_FONT_W) + osdInsideX();
    ESPectrum::vga.setCursor(x, y);
}

void OSD::drawOSD() {
    VGA& vga = ESPectrum::vga;
    unsigned short x = scrAlignCenterX(OSD_W);
    unsigned short y = scrAlignCenterY(OSD_H);
    vga.fillRect(x, y, OSD_W, OSD_H, OSD::zxColor(1, 0));
    vga.rect(x, y, OSD_W, OSD_H, OSD::zxColor(0, 0));
    vga.rect(x + 1, y + 1, OSD_W - 2, OSD_H - 2, OSD::zxColor(7, 0));
    vga.setTextColor(OSD::zxColor(0, 0), OSD::zxColor(5, 1));
    vga.setFont(Font6x8);
    osdHome();
    vga.print(OSD_TITLE);
    osdAt(17, 0);
    vga.print(OSD_BOTTOM);
    osdHome();
}

static void quickSave()
{
    OSD::osdCenteredMsg(OSD_QSNA_SAVING, LEVEL_INFO);
    if (!FileSNA::saveQuick()) {
        OSD::osdCenteredMsg(OSD_QSNA_SAVE_ERR, LEVEL_WARN);
        delay(1000);
        return;
    }
    OSD::osdCenteredMsg(OSD_QSNA_SAVED, LEVEL_INFO);
    delay(200);
}

static void quickLoad()
{
    if (!FileSNA::isQuickAvailable()) {
        OSD::osdCenteredMsg(OSD_QSNA_NOT_AVAIL, LEVEL_INFO);
        delay(1000);
        return;
    }
    OSD::osdCenteredMsg(OSD_QSNA_LOADING, LEVEL_INFO);
    if (!FileSNA::loadQuick()) {
        OSD::osdCenteredMsg(OSD_QSNA_LOAD_ERR, LEVEL_WARN);
        delay(1000);
        return;
    }
    if (Config::getArch() == "48K") AySound::reset();
    if (Config::getArch() == "48K") ESPectrum::samplesPerFrame=546; else ESPectrum::samplesPerFrame=554;
    OSD::osdCenteredMsg(OSD_QSNA_LOADED, LEVEL_INFO);
    delay(200);
}

static void persistSave(byte slotnumber)
{
    char persistfname[strlen(DISK_PSNA_FILE) + 6];
    sprintf(persistfname,DISK_PSNA_FILE "%u.sna",slotnumber);
    OSD::osdCenteredMsg(OSD_PSNA_SAVING, LEVEL_INFO);
    if (!FileSNA::save(persistfname)) {
        OSD::osdCenteredMsg(OSD_PSNA_SAVE_ERR, LEVEL_WARN);
        delay(1000);
        return;
    }
    OSD::osdCenteredMsg(OSD_PSNA_SAVED, LEVEL_INFO);
    delay(400);
}

static void persistLoad(byte slotnumber)
{
    char persistfname[strlen(DISK_PSNA_FILE) + 6];
    sprintf(persistfname,DISK_PSNA_FILE "%u.sna",slotnumber);
    if (!FileSNA::isPersistAvailable(persistfname)) {
        OSD::osdCenteredMsg(OSD_PSNA_NOT_AVAIL, LEVEL_INFO);
        delay(1000);
        return;
    }
    OSD::osdCenteredMsg(OSD_PSNA_LOADING, LEVEL_INFO);
    if (!FileSNA::load(persistfname)) {
         OSD::osdCenteredMsg(OSD_PSNA_LOAD_ERR, LEVEL_WARN);
         delay(1000);
    }
    if (Config::getArch() == "48K") AySound::reset();
    if (Config::getArch() == "48K") ESPectrum::samplesPerFrame=546; else ESPectrum::samplesPerFrame=554;
    OSD::osdCenteredMsg(OSD_PSNA_LOADED, LEVEL_INFO);
    delay(400);
}

// OSD Main Loop
void OSD::do_OSD() {
    VGA& vga = ESPectrum::vga;
    static byte last_sna_row = 0;
    static unsigned int last_demo_ts = millis() / 1000;
    if (PS2Keyboard::checkAndCleanKey(KEY_PAUSE)) {
        AySound::disable();
        osdCenteredMsg(OSD_PAUSE, LEVEL_INFO);
        while (!PS2Keyboard::checkAndCleanKey(KEY_PAUSE)) {
            delay(5);
        }
        AySound::enable();
    }
    else if (PS2Keyboard::checkAndCleanKey(KEY_F2)) {
        AySound::disable();
        quickSave();
        AySound::enable();
    }
    else if (PS2Keyboard::checkAndCleanKey(KEY_F3)) {
        AySound::disable();
        quickLoad();
        AySound::enable();
    }
    else if (PS2Keyboard::checkAndCleanKey(KEY_F4)) {
        AySound::disable();
        // Persist Save
        byte opt2 = menuRun(MENU_PERSIST_SAVE);
        if (opt2 > 0 && opt2<6) {
            persistSave(opt2);
        }
        AySound::enable();
    }
    else if (PS2Keyboard::checkAndCleanKey(KEY_F5)) {
        AySound::disable();
        // Persist Load
        byte opt2 = menuRun(MENU_PERSIST_LOAD);
        if (opt2 > 0 && opt2<6) {
            persistLoad(opt2);
        }
        AySound::enable();
    }
    else if (PS2Keyboard::checkAndCleanKey(KEY_F6)) {
        // Start .tap reproduction
        if (Tape::tapeFileName=="none") {
            OSD::osdCenteredMsg(OSD_TAPE_SELECT_ERR, LEVEL_WARN);
            delay(1000);                
        } else {
            Tape::TAP_Play();
        }
    }
    else if (PS2Keyboard::checkAndCleanKey(KEY_F7)) {
        // Stop .tap reproduction
        Tape::tapeStatus=TAPE_STOPPED;
    }
    else if (PS2Keyboard::checkAndCleanKey(KEY_F9)) {
        if (ESPectrum::aud_volume>-16) {
                ESPectrum::aud_volume--;
                pwm_audio_set_volume(ESPectrum::aud_volume);
        }
    }
    else if (PS2Keyboard::checkAndCleanKey(KEY_F10)) {
        if (ESPectrum::aud_volume<0) {
                ESPectrum::aud_volume++;
                pwm_audio_set_volume(ESPectrum::aud_volume);
        }
    }    
/*  // Testing code
    else if (PS2Keyboard::checkAndCleanKey(KEY_F11)) {
        ESPectrum::ESPoffset-=50;
    }    
    else if (PS2Keyboard::checkAndCleanKey(KEY_F12)) {
        ESPectrum::ESPoffset+=50;
    }    
*/
    else if (PS2Keyboard::checkAndCleanKey(KEY_F1)) {
        AySound::disable();

        // Main menu
        byte opt = menuRun(MENU_MAIN);
        if (opt == 1) {
            // Change RAM
            unsigned short snanum = menuRun(Config::sna_name_list);
            if (snanum > 0) {
                changeSnapshot(rowGet(Config::sna_file_list, snanum));
            }
        } else if (opt == 2) {
            // Change TAP
            unsigned short tapnum = menuRun(Config::tap_name_list);
            if (tapnum > 0) {
                Tape::tapeFileName=DISK_TAP_DIR "/" + rowGet(Config::tap_file_list, tapnum);
                if (!Tape::TAP_Load()) {
                    Tape::tapeFileName="none";
                    OSD::osdCenteredMsg(OSD_TAPE_LOAD_ERR, LEVEL_WARN);
                    delay(1000);
                } else menuRun(MENU_TAP_SELECTED);
            }
        }
        else if (opt == 3) {
            // Change ROM
            String arch_menu = getArchMenu();
            byte arch_num = menuRun(arch_menu);
            if (arch_num > 0) {
                String romset_menu = getRomsetMenu(rowGet(arch_menu, arch_num));
                byte romset_num = menuRun(romset_menu);
                if (romset_num > 0) {
                    String arch = rowGet(arch_menu, arch_num);
                    String romSet = rowGet(romset_menu, romset_num);
                    Config::requestMachine(arch, romSet, true);
                    vTaskDelay(2);

                    Config::save();
                    vTaskDelay(2);
                    ESPectrum::reset();
                }
            }
        }
        else if (opt == 4) {
            quickSave();
        }
        else if (opt == 5) {
            quickLoad();
        }
        else if (opt == 6) {
            // Persist Save
            byte opt2 = menuRun(MENU_PERSIST_SAVE);
            if (opt2 > 0 && opt2<6) {
                persistSave(opt2);
            }
        }
        else if (opt == 7) {
            // Persist Load
            byte opt2 = menuRun(MENU_PERSIST_LOAD);
            if (opt2 > 0 && opt2<6) {
                persistLoad(opt2);
            }
        }
        else if (opt == 8) {
            // aspect ratio
            byte opt2;
            if (Config::aspect_16_9)
                opt2 = menuRun(MENU_ASPECT_169);
            else
                opt2 = menuRun(MENU_ASPECT_43);
            if (opt2 == 2)
            {
                Config::aspect_16_9 = !Config::aspect_16_9;
                Config::save();
                ESP.restart();
            }
        }
        else if (opt == 9) {
            // Reset
            byte opt2 = menuRun(MENU_RESET);
            if (opt2 == 1) {
                // Soft
                ESPectrum::reset();
                if (Config::ram_file != (String)NO_RAM_FILE)
                    FileSNA::load("/sna/" + Config::ram_file); // TO DO: fix error if ram_file is .z80 
            }
            else if (opt2 == 2) {
                // Hard
                Config::ram_file = (String)NO_RAM_FILE;
                Config::save();
                ESPectrum::reset();
                ESP.restart();
            }
            else if (opt2 == 3) {
                // ESP host reset
                ESP.restart();
            }
        }
        else if (opt == 10) {
            // Help
            drawOSD();
            osdAt(2, 0);
            vga.setTextColor(OSD::zxColor(7, 0), OSD::zxColor(1, 0));
            vga.print(OSD_HELP);
            while (!PS2Keyboard::checkAndCleanKey(KEY_F1) &&
                   !PS2Keyboard::checkAndCleanKey(KEY_ESC) &&
                   !PS2Keyboard::checkAndCleanKey(KEY_ENTER)) {
                vTaskDelay(5);
                updateWiimote2KeysOSD();
            }
        }
        
        AySound::enable();
        // Exit
    }
}

// Shows a red panel with error text
void OSD::errorPanel(String errormsg) {
    unsigned short x = scrAlignCenterX(OSD_W);
    unsigned short y = scrAlignCenterY(OSD_H);

    if (Config::slog_on)
        Serial.println(errormsg);

    VGA& vga = ESPectrum::vga;

    vga.fillRect(x, y, OSD_W, OSD_H, OSD::zxColor(0, 0));
    vga.rect(x, y, OSD_W, OSD_H, OSD::zxColor(7, 0));
    vga.rect(x + 1, y + 1, OSD_W - 2, OSD_H - 2, OSD::zxColor(2, 1));
    vga.setFont(Font6x8);
    osdHome();
    vga.setTextColor(OSD::zxColor(7, 1), OSD::zxColor(2, 1));
    vga.print(ERROR_TITLE);
    osdAt(2, 0);
    vga.setTextColor(OSD::zxColor(7, 1), OSD::zxColor(0, 0));
    vga.println(errormsg.c_str());
    osdAt(17, 0);
    vga.setTextColor(OSD::zxColor(7, 1), OSD::zxColor(2, 1));
    vga.print(ERROR_BOTTOM);
}

// Error panel and infinite loop
void OSD::errorHalt(String errormsg) {
    errorPanel(errormsg);
    while (1) {
        ESPectrum::processKeyboard();
        do_OSD();
        delay(5);
    }
}

// Centered message
void OSD::osdCenteredMsg(String msg, byte warn_level) {
    const unsigned short w = (msg.length() + 2) * OSD_FONT_W;
    const unsigned short h = OSD_FONT_H * 3;
    const unsigned short x = scrAlignCenterX(w);
    const unsigned short y = scrAlignCenterY(h);
    unsigned short paper;
    unsigned short ink;

    switch (warn_level) {
    case LEVEL_OK:
        ink = OSD::zxColor(7, 1);
        paper = OSD::zxColor(4, 0);
        break;
    case LEVEL_ERROR:
        ink = OSD::zxColor(7, 1);
        paper = OSD::zxColor(2, 0);
        break;
    case LEVEL_WARN:
        ink = OSD::zxColor(0, 0);
        paper = OSD::zxColor(6, 0);
        break;
    default:
        ink = OSD::zxColor(7, 0);
        paper = OSD::zxColor(1, 0);
    }

    VGA& vga = ESPectrum::vga;

    vga.fillRect(x, y, w, h, paper);
    // vga.rect(x - 1, y - 1, w + 2, h + 2, ink);
    vga.setTextColor(ink, paper);
    vga.setFont(Font6x8);
    vga.setCursor(x + OSD_FONT_W, y + OSD_FONT_H);
    vga.print(msg.c_str());
}

// Count NL chars inside a string, useful to count menu rows
unsigned short OSD::rowCount(String menu) {
    unsigned short count = 0;
    for (unsigned short i = 1; i < menu.length(); i++) {
        if (menu.charAt(i) == ASCII_NL) {
            count++;
        }
    }
    return count;
}

// Get a row text
String OSD::rowGet(String menu, unsigned short row) {
    unsigned short count = 0;
    unsigned short last = 0;
    for (unsigned short i = 0; i < menu.length(); i++) {
        if (menu.charAt(i) == ASCII_NL) {
            if (count == row) {
                return menu.substring(last, i);
            }
            count++;
            last = i + 1;
        }
    }
    return "MENU ERROR! (Unknown row?)";
}

#include "FileSNA.h"
#include "FileZ80.h"

// Change running snapshot
void OSD::changeSnapshot(String filename)
{
    if (FileUtils::hasSNAextension(filename))
    {
        osdCenteredMsg((String)MSG_LOADING_SNA + ": " + filename, LEVEL_INFO);
        ESPectrum::reset();
        Serial.printf("Loading SNA: %s\n", filename.c_str());
        FileSNA::load((String)DISK_SNA_DIR + "/" + filename);
    }
    else if (FileUtils::hasZ80extension(filename))
    {
        osdCenteredMsg((String)MSG_LOADING_Z80 + ": " + filename, LEVEL_INFO);
        ESPectrum::reset();
        Serial.printf("Loading Z80: %s\n", filename.c_str());
        FileZ80::load((String)DISK_SNA_DIR + "/" + filename);
    }
    osdCenteredMsg(MSG_SAVE_CONFIG, LEVEL_WARN);
    Config::ram_file = filename;
    Config::save();

    if (Config::getArch() == "48K") AySound::reset();
    if (Config::getArch() == "48K") ESPectrum::samplesPerFrame=546; else ESPectrum::samplesPerFrame=554;    
}
