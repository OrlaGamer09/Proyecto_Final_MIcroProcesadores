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

#ifndef ESPectrum_hardpins_h
#define ESPectrum_hardpins_h

#include "hardconfig.h"

// adjusted for Lilygo TTGO
#ifdef SPEAKER_PRESENT
// NOTE: PIN 25 is hardwired in FabGL audio (used for AY3891x emulation)
#define SPEAKER_PIN 25
#endif // SPEAKER_PRESENT

#ifdef EAR_PRESENT
#define EAR_PIN 16
#endif // EAR_PRESENT

#ifdef MIC_PRESENT
#define MIC_PIN 17
#endif // MIC_PRESENT

#ifdef PS2_KEYB_PRESENT
// adjusted for Lilygo TTGO
#define KEYBOARD_DATA 32
#define KEYBOARD_CLK 33
#endif // PS2_KEYB_PRESENT

#ifdef ZX_KEYB_PRESENT
#define AD8 12
#define AD9 26
#define AD10 25
#define AD11 33
#define AD12 27
#define AD13 14
#define AD14 0
#define AD15 13

#define DB0 36
#define DB1 39
#define DB2 34
#define DB3 35
#define DB4 32
#endif // ZX_KEYB_PRESENT

// Storage mode: pins for external SD card
#ifdef USE_SD_CARD
// adjusted for Lilygo TTGO
#define SDCARD_CS 13
#define SDCARD_MOSI 12
#define SDCARD_MISO 2
#define SDCARD_CLK 14
#endif

// 3 bit pins
#ifdef COLOR_3B
#define RED_PIN_3B 22
#define GRE_PIN_3B 19
#define BLU_PIN_3B 5
#endif // COLOR_3B

// 6 bit pins
#ifdef COLOR_6B
// adjusted for Lilygo TTGO
#define RED_PINS_6B 21, 22
#define GRE_PINS_6B 18, 19
#define BLU_PINS_6B  4,  5
#endif // COLOR_6B

// 14 bit pins
#ifdef COLOR_14B
#define RED_PINS_14B 21, 21, 21, 21, 22
#define GRE_PINS_14B 18, 18, 18, 18, 19
#define BLU_PINS_14B      4,  4,  4,  5
#endif // COLOR_14B

// VGA sync pins
#define HSYNC_PIN 23
#define VSYNC_PIN 15

/////////////////////////////////////////////////
// Colors for 3 bit mode
#ifdef COLOR_3B           //       BGR 
#define BLACK       0x08      // 0000 1000
#define BLUE        0x0C      // 0000 1100
#define RED         0x09      // 0000 1001
#define MAGENTA     0x0D      // 0000 1101
#define GREEN       0x0A      // 0000 1010
#define CYAN        0x0E      // 0000 1110
#define YELLOW      0x0B      // 0000 1011
#define WHITE       0x0F      // 0000 1111

#define BRI_BLACK   0x08      // 0000 1000
#define BRI_BLUE    0x0C      // 0000 1100
#define BRI_RED     0x09      // 0000 1001
#define BRI_MAGENTA 0x0D      // 0000 1101
#define BRI_GREEN   0x0A      // 0000 1010
#define BRI_CYAN    0x0E      // 0000 1110
#define BRI_YELLOW  0x0B      // 0000 1011
#define BRI_WHITE   0x0F      // 0000 1111
#endif

/////////////////////////////////////////////////
// Colors for 6 bit mode
#ifdef COLOR_6B               //   BB GGRR 
#define BLACK       0xC0      // 1100 0000
#define BLUE        0xE0      // 1110 0000
#define RED         0xC2      // 1100 0010
#define MAGENTA     0xE2      // 1110 0010
#define GREEN       0xC8      // 1100 1000
#define CYAN        0xE8      // 1110 1000
#define YELLOW      0xCA      // 1100 1010
#define WHITE       0xEA      // 1110 1010
                              //   BB GGRR 
#define BRI_BLACK   0xC0      // 1100 0000
#define BRI_BLUE    0xF0      // 1111 0000
#define BRI_RED     0xC3      // 1100 0011
#define BRI_MAGENTA 0xF3      // 1111 0011
#define BRI_GREEN   0xCC      // 1100 1100
#define BRI_CYAN    0xFC      // 1111 1100
#define BRI_YELLOW  0xCF      // 1100 1111
#define BRI_WHITE   0xFF      // 1111 1111
#endif

/////////////////////////////////////////////////
// Colors for 14 bit mode
#ifdef COLOR_14B              //   BB --GG ---R R---
#define BRI_BLACK   0xC000    // 1100 0000 0000 0000
#define BRI_BLUE    0xF000    // 1111 0000 0000 0000
#define BRI_RED     0xC018    // 1100 0000 0001 1000
#define BRI_MAGENTA 0xF018    // 1111 0000 0001 1000
#define BRI_GREEN   0xC300    // 1100 0011 0000 0000
#define BRI_CYAN    0xF300    // 1111 0011 0000 0000
#define BRI_YELLOW  0xC318    // 1100 0011 0001 1000
#define BRI_WHITE   0xF318    // 1111 0011 0001 1000
                              //   BB --GG ---R R---
#define BLACK       0xC000    // 1100 0000 0000 0000
#define BLUE        0xE000    // 1110 0000 0000 0000
#define RED         0xC010    // 1100 0000 0001 0000
#define MAGENTA     0xE010    // 1110 0000 0001 0000
#define GREEN       0xC200    // 1100 0010 0000 0000
#define CYAN        0xE200    // 1110 0010 0000 0000
#define YELLOW      0xC210    // 1100 0010 0001 0000
#define WHITE       0xE210    // 1110 0010 0001 0000

#endif

#endif // ESPectrum_hardpins_h
