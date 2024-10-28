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

#ifndef ESPectrum_h
#define ESPectrum_h

#include "hardpins.h"
#include <FS.h>

// Declared vars
#ifdef COLOR_3B
#include "ESP32Lib/VGA/VGA3Bit.h"
#include "ESP32Lib/VGA/VGA3BitI.h"
#define VGA VGA3Bit
#endif

#ifdef COLOR_6B
#include "ESP32Lib/VGA/VGA6Bit.h"
#include "ESP32Lib/VGA/VGA6BitI.h"
#define VGA VGA6Bit
#endif

// #ifdef COLOR_6B
// #include "vga_6bit.h"
// #define VGA VGA6Bit
// #endif

#ifdef COLOR_14B
#include "ESP32Lib/VGA/VGA14Bit.h"
#include "ESP32Lib/VGA/VGA14BitI.h"
#define VGA VGA14Bit
#endif

#define ESP_AUDIO_OVERSAMPLES 4432 // For 48K we get 4368 samples per frame, for 128K we get 4432

#define ESP_AUDIO_FREQ 27300
#define ESP_AUDIO_SAMPLES 554 // For 48K we get 546 samples per frame, for 128K we get 554
#define ESP_AUDIO_TSTATES 128

// #define ESP_AUDIO_FREQ 13650
// #define ESP_AUDIO_SAMPLES 273
// #define ESP_AUDIO_TSTATES 256

class ESPectrum
{
public:

    // arduino setup/loop
    static void setup();
    static void loop();

    // reset machine
    static void reset();

    // Video
    static VGA vga;
    static uint8_t borderColor;
    static void processKeyboard();

    // Audio
    static unsigned char audioBuffer[2][ESP_AUDIO_SAMPLES];
    static unsigned char overSamplebuf[ESP_AUDIO_OVERSAMPLES];
    static signed char aud_volume;
    static int buffertofill;
    static int buffertoplay;
    static uint32_t audbufcnt;
    static int lastaudioBit;
    static void audioFrameStart();
    static void audioGetSample(int Audiobit);
    static void audioFrameEnd();
    static int samplesPerFrame;


    //static int ESPoffset; // Testing
   
private:

    static void audioTask(void* unused);

};

#endif