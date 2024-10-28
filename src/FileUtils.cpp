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
#include "CPU.h"
#include "Mem.h"
#include "ESPectrum.h"
#include "hardpins.h"
#include "messages.h"
#include "osd.h"
#include <FS.h>
#include "Wiimote2Keys.h"
#include "sort.h"
#include "FileUtils.h"
#include "Config.h"

#ifdef USE_INT_FLASH
// using internal storage (spi flash)
#include <SPIFFS.h>
// set The Filesystem to SPIFFS
#define THE_FS SPIFFS
#endif

#ifdef USE_SD_CARD
// using external storage (SD card)
#include <SD.h>
// set The Filesystem to SD
#define THE_FS SD
static SPIClass customSPI;
#endif

void zx_reset();

// Globals
//void IRAM_ATTR FileUtils::initFileSystem() {
void FileUtils::initFileSystem() {
#ifdef USE_INT_FLASH
// using internal storage (spi flash)
    Serial.println("Initializing internal storage...");
    if (!SPIFFS.begin())
        OSD::errorHalt(ERR_FS_INT_FAIL);

    vTaskDelay(2);

#endif
#ifdef USE_SD_CARD
// using external storage (SD card)

    Serial.println("Initializing external storage...");

    customSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);

    if (!SD.begin(SDCARD_CS, customSPI, 4000000, "/sd")) {
        Serial.println("Card Mount Failed");
        OSD::errorHalt(ERR_FS_EXT_FAIL);
        return;
    }
    uint8_t cardType = SD.cardType();
    
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        OSD::errorHalt(ERR_FS_EXT_FAIL);
        return;
    }
    
    Serial.print("SD Card Type: ");
    if      (cardType == CARD_MMC)  Serial.println("MMC");
    else if (cardType == CARD_SD )  Serial.println("SDSC");
    else if (cardType == CARD_SDHC) Serial.println("SDHC");
    else                            Serial.println("UNKNOWN");
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    vTaskDelay(2);
#endif
}

String FileUtils::getAllFilesFrom(const String path) {
    KB_INT_STOP;
    File root = THE_FS.open("/");
    File file = root.openNextFile();
    String listing;

    while (file) {
        file = root.openNextFile();
        String filename = file.name();
        if (filename.startsWith(path) && !filename.startsWith(path + "/.")) {
            listing.concat(filename.substring(path.length() + 1));
            listing.concat("\n");
        }
    }
    vTaskDelay(2);
    KB_INT_START;
    return listing;
}

void FileUtils::listAllFiles() {
    KB_INT_STOP;
    File root = THE_FS.open("/");
    Serial.println("fs opened");
    File file = root.openNextFile();
    Serial.println("fs openednextfile");

    while (file) {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
    vTaskDelay(2);
    KB_INT_START;
}

void FileUtils::sanitizeFilename(String filename)
{
    filename.replace("\n", " ");
    filename.trim();
}

File FileUtils::safeOpenFileRead(String filename)
{
    sanitizeFilename(filename);
    File f;
    if (Config::slog_on)
        Serial.printf("%s '%s'\n", MSG_LOADING, filename.c_str());
    if (!THE_FS.exists(filename.c_str())) {
        KB_INT_START;
        OSD::errorHalt((String)ERR_READ_FILE + "\n" + filename);
    }
    f = THE_FS.open(filename.c_str(), FILE_READ);
    vTaskDelay(2);

    return f;
}

String FileUtils::getFileEntriesFromDir(String path) {
    KB_INT_STOP;
    Serial.printf("Getting entries from: '%s'\n", path.c_str());
    String filelist;
    File root = THE_FS.open(path.c_str());
    if (!root || !root.isDirectory()) {
        OSD::errorHalt((String)ERR_DIR_OPEN + "\n" + root);
    }
    File file = root.openNextFile();
    if (!file)
        Serial.println("No entries found!");
    while (file) {
        Serial.printf("Found %s: %s...%ub...", (file.isDirectory() ? "DIR" : "FILE"), file.name(), file.size());
        String filename = file.name();
        byte start = filename.indexOf("/", path.length()) + 1;
        byte end = filename.indexOf("/", start);
        filename = filename.substring(start, end);
        Serial.printf("%s...", filename.c_str());
        if (filename.startsWith(".")) {
            Serial.println("HIDDEN");
        } else if (filename.endsWith(".txt")) {
            Serial.println("IGNORING TXT");
//        } else if (Config::arch == "48K" & file.size() > SNA_48K_SIZE) {
//            Serial.println("128K SKIP");
        } else {
            if (filelist.indexOf(filename) < 0) {
                Serial.println("ADDING");
                filelist += filename + "\n";
            } else {
                Serial.println("EXISTS");
            }
        }
        file = root.openNextFile();
    }
    KB_INT_START;
    return filelist;
}

bool FileUtils::hasSNAextension(String filename)
{
    if (filename.endsWith(".sna")) return true;
    if (filename.endsWith(".SNA")) return true;
    return false;
}

bool FileUtils::hasZ80extension(String filename)
{
    if (filename.endsWith(".z80")) return true;
    if (filename.endsWith(".Z80")) return true;
    return false;
}


uint16_t FileUtils::countFileEntriesFromDir(String path) {
    String entries = getFileEntriesFromDir(path);
    unsigned short count = 0;
    for (unsigned short i = 0; i < entries.length(); i++) {
        if (entries.charAt(i) == ASCII_NL) {
            count++;
        }
    }
    return count;
}

void FileUtils::loadRom(String arch, String romset) {
    KB_INT_STOP;
    String path = "/rom/" + arch + "/" + romset;
    Serial.printf("Loading ROMSET '%s'\n", path.c_str());
    byte n_roms = countFileEntriesFromDir(path);
    if (n_roms < 1) {
        OSD::errorHalt("No ROMs found at " + path + "\nARCH: '" + arch + "' ROMSET: " + romset);
    }
    Serial.printf("Processing %u ROMs\n", n_roms);
    for (byte f = 0; f < n_roms; f++) {
        File rom_f = FileUtils::safeOpenFileRead(path + "/" + (String)f + ".rom");
        Serial.printf("Loading ROM '%s'\n", rom_f.name());
        for (int i = 0; i < rom_f.size(); i++) {
            switch (f) {
            case 0:
                Mem::rom0[i] = rom_f.read();
                break;
            case 1:
                Mem::rom1[i] = rom_f.read();
                break;

#ifdef BOARD_HAS_PSRAM

            case 2:
                Mem::rom2[i] = rom_f.read();
                break;
            case 3:
                Mem::rom3[i] = rom_f.read();
                break;
#endif
            }
        }
        rom_f.close();
    }

    KB_INT_START;
}

// Get all sna files sorted alphabetically
String FileUtils::getSortedFileList(String fileDir)
{
    // get string of unsorted filenames, separated by newlines
    String entries = getFileEntriesFromDir(fileDir);

    // count filenames (they always end at newline)
    unsigned short count = 0;
    for (unsigned short i = 0; i < entries.length(); i++) {
        if (entries.charAt(i) == ASCII_NL) {
            count++;
        }
    }

    // array of filenames
    String* filenames = (String*)malloc(count * sizeof(String));
    // memory must be initialized to avoid crash on assign
    memset(filenames, 0, count * sizeof(String));

    // copy filenames from string to array
    unsigned short ich = 0;
    unsigned short ifn = 0;
    for (unsigned short i = 0; i < entries.length(); i++) {
        if (entries.charAt(i) == ASCII_NL) {
            filenames[ifn++] = entries.substring(ich, i);
            ich = i + 1;
        }
    }

    // sort array
    sortArray(filenames, count);

    // string of sorted filenames
    String sortedEntries = "";

    // copy filenames from array to string
    for (unsigned short i = 0; i < count; i++) {
        sortedEntries += filenames[i] + '\n';
    }

    return sortedEntries;
}
