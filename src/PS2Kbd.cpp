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

#include "hardconfig.h"

#include "ESPectrum.h"
#include "hardpins.h"
#include <Arduino.h>
#include "PS2Kbd.h"

static boolean keyup = false;

uint8_t PS2Keyboard::keymap[256];
uint8_t PS2Keyboard::oldmap[256];

// #define DEBUG_LOG_KEYSTROKES 1

#ifdef PS2_KEYB_PRESENT

#ifdef  PS2_KEYB_FORCE_INIT
#include "PS2Boot/PS2KeyAdvanced.h"
PS2KeyAdvanced ps2boot;
#endif

void PS2Keyboard::initialize()
{
#ifdef  PS2_KEYB_FORCE_INIT
    // Configure the keyboard library
    ps2boot.begin( KEYBOARD_DATA, KEYBOARD_CLK );
    ps2boot.echo( );              // ping keyboard to see if there
    delay( 6 );
    ps2boot.read( );
    delay( 6 );
    ps2boot.terminate();
#endif

    pinMode(KEYBOARD_DATA, INPUT_PULLUP);
    pinMode(KEYBOARD_CLK, INPUT_PULLUP);
    digitalWrite(KEYBOARD_DATA, true);
    digitalWrite(KEYBOARD_CLK, true);
    attachInterrupt();

    memset(keymap, 1, sizeof(keymap));
    memset(oldmap, 1, sizeof(oldmap));
}

void IRAM_ATTR PS2Keyboard::interruptHandler(void)
{
    static uint8_t bitcount = 0;
    static uint8_t incoming = 0;
    static uint32_t prev_ms = 0;
    uint32_t now_ms;
    uint8_t n, val;

    int clock = digitalRead(KEYBOARD_CLK);
    if (clock == 1)
        return;

    val = digitalRead(KEYBOARD_DATA);
    now_ms = millis();
    if (now_ms - prev_ms > 250) {
        bitcount = 0;
        incoming = 0;
    }
    prev_ms = now_ms;
    n = bitcount - 1;
    if (n <= 7) {
        incoming |= (val << n);
    }
    bitcount++;
    if (bitcount == 11) {

        if (1) {
            if (keyup == true) {
                if (keymap[incoming] == 0) {
                    keymap[incoming] = 1;
                } else {
                    // Serial.println("WARNING: Keyboard cleaned");
                    for (int gg = 0; gg < 256; gg++)
                        keymap[gg] = 1;
                }
                keyup = false;
            } else
                keymap[incoming] = 0;

#ifdef DEBUG_LOG_KEYSTROKES
                Serial.printf("PS2Kbd[%s]: %02X\n",
                    keyup ? " up " : "down", incoming);
#endif

            if (incoming == 240)
                keyup = true;
            else
                keyup = false;
        }
        bitcount = 0;
        incoming = 0;
    }
}

void PS2Keyboard::attachInterrupt()
{
    ::attachInterrupt(digitalPinToInterrupt(KEYBOARD_CLK), PS2Keyboard::interruptHandler, FALLING);
}

void PS2Keyboard::detachInterrupt()
{
    ::detachInterrupt(digitalPinToInterrupt(KEYBOARD_CLK));
}

#else // ! PS2_KEYB_PRESENT

void PS2Keyboard::begin()
{
    memset(keymap, 1, sizeof(keymap));
    memset(oldmap, 1, sizeof(oldmap));
}

void IRAM_ATTR PS2Keyboard::interruptHandler(void)
{
}

void PS2Keyboard::attachInterrupt()
{
}

void PS2Keyboard::detachInterrupt()
{
}


#endif // PS2_KEYB_PRESENT

// Check if keymatrix is changed
boolean PS2Keyboard::isKeymapChanged() { return (keymap != oldmap); } //?

// Check if key is pressed and clean it
boolean PS2Keyboard::checkAndCleanKey(uint8_t scancode) {
    if (keymap[scancode] == 0) {
        keymap[scancode] = 1;
        return true;
    }
    return false;
}

void PS2Keyboard::emulateKeyChange(uint8_t scancode, uint8_t isdown)
{
    keymap[scancode] = isdown ? 0 : 1;
}

