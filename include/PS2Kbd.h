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

 #ifndef ESPECTRUM_PS2KBD_H
 #define ESPECTRUM_PS2KBD_H

#include "hardconfig.h"

#include "hardpins.h"
#include <Arduino.h>

class PS2Keyboard
{
public:
    static void initialize();
    static void IRAM_ATTR interruptHandler(void);
    static bool isKeymapChanged();
    static bool checkAndCleanKey(uint8_t scancode);

    static void attachInterrupt();
    static void detachInterrupt();

    static uint8_t keymap[256];
    static uint8_t oldmap[256];

    // inject key from wiimote, for not modifying OSD code
    static void emulateKeyChange(uint8_t scancode, uint8_t isdown);
};

#ifdef PS2_KEYB_PRESENT

#define KB_INT_START PS2Keyboard::attachInterrupt()
#define KB_INT_STOP  PS2Keyboard::detachInterrupt()

#else // !PS2_KEYB_PRESENT

#define KB_INT_START
#define KB_INT_STOP

#endif // PS2_KEYB_PRESENT

// PS/2 scan codes for OSD / special keys
#define KEY_F1  0x05
#define KEY_F2  0x06
#define KEY_F3  0x04
#define KEY_F4  0x0C
#define KEY_F5  0x03
#define KEY_F6  0x0B
#define KEY_F7  0x83
#define KEY_F8  0x0A
#define KEY_F9  0x01
#define KEY_F10 0x09
#define KEY_F11 0x78
#define KEY_F12 0x07
#define KEY_ESC 0x76
#define KEY_CURSOR_LEFT  0x6B
#define KEY_CURSOR_DOWN  0x72
#define KEY_CURSOR_RIGHT 0x74
#define KEY_CURSOR_UP    0x75
#define KEY_ALT_GR       0x11
#define KEY_ENTER        0x5A
#define KEY_HOME         0xE06C
#define KEY_END          0xE069
#define KEY_PAGE_UP      0xE07D
#define KEY_PAGE_DOWN    0xE07A
#define KEY_PAUSE        0xE11477E1F014E077

#define KEY_COMMA     0x41
#define KEY_DOT       0x49
#define KEY_DIV       0x4A
#define KEY_SEMI      0x4C
#define KEY_MINUS     0x4E
#define KEY_APOS      0x52
#define KEY_EQUAL     0x55
#define KEY_BACKSPACE 0x66

#endif // ESPECTRUM_PS2KBD_H
