/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Ha Thach (tinyusb.org) for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CANARIN_FLASH_H_
#define CANARIN_FLASH_H_

#include "SdFat.h"
#include "Adafruit_SPIFlash.h"

// Un-comment to run example with custom SPI and SS e.g with FRAM breakout
// #define CUSTOM_CS   A5
// #define CUSTOM_SPI  SPI

#if defined(CUSTOM_CS) && defined(CUSTOM_SPI)
Adafruit_FlashTransport_SPI flashTransport(CUSTOM_CS, CUSTOM_SPI);

#elif defined(ARDUINO_ARCH_ESP32)

// ESP32 use same flash device that store code for file system.
// SPIFlash will parse partition.cvs to detect FATFS partition to use
Adafruit_FlashTransport_ESP32 flashTransport;

#elif defined(ARDUINO_ARCH_RP2040)

// RP2040 use same flash device that store code for file system. Therefore we
// only need to specify start address and size (no need SPI or SS)
// By default (start=0, size=0), values that match file system setting in
// 'Tools->Flash Size' menu selection will be used.
Adafruit_FlashTransport_RP2040 flashTransport;

// To be compatible with CircuitPython partition scheme (start_address = 1 MB,
// size = total flash - 1 MB) use const value (CPY_START_ADDR, CPY_SIZE) or
// subclass Adafruit_FlashTransport_RP2040_CPY. Un-comment either of the
// following line:
//  Adafruit_FlashTransport_RP2040
//    flashTransport(Adafruit_FlashTransport_RP2040::CPY_START_ADDR,
//                   Adafruit_FlashTransport_RP2040::CPY_SIZE);
//  Adafruit_FlashTransport_RP2040_CPY flashTransport;

#else

// On-board external flash (QSPI or SPI) macros should already
// defined in your board variant if supported
// - EXTERNAL_FLASH_USE_QSPI
// - EXTERNAL_FLASH_USE_CS/EXTERNAL_FLASH_USE_SPI

#if defined(EXTERNAL_FLASH_USE_QSPI)

Adafruit_FlashTransport_QSPI flashTransport;

#elif defined(EXTERNAL_FLASH_USE_SPI)

Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS,
                                           EXTERNAL_FLASH_USE_SPI);

#elif defined(__AVR__)

// Use stand SPI/SS for avr port.
// Note cache will be disable due to lack of memory.

Adafruit_FlashTransport_SPI flashTransport(SS, SPI);

#else
#error No (Q)SPI flash are defined for your board !
#endif

#endif



#define LED_GPIO             12//BV 13
#define PIN_RGB             16
#define LED_ON               HIGH
#define LED_OFF              LOW
Adafruit_NeoPixel pixels(1, PIN_RGB, NEO_GRB + NEO_KHZ800);
 
//Adafruit_FlashTransport_SPI flashTransport(8, SPI); // Set CS and SPI interface
Adafruit_SPIFlash flash(&flashTransport);
 
// file system object from SdFat
FatFileSystem fatfs;
File myFile;



bool flash_setup = false;


void Canarin_Flash_setup(){
  if (flash_setup)
    return;

  SPI.begin(45, 48, 47, 8);
  
  if (flash.begin()) {
      logger.println(F("Device found and supported!"));
  } else {
      logger.println(F("Problem to discover and configure device, check wiring also!"));
  }
  // Set 24Mhz SPI speed
  flashTransport.setClockSpeed(24000000, 24000000); // added to prevent speed problem
 
  pinMode(LED_GPIO, OUTPUT);
  //flash.setIndicator(LED_GPIO, true);
  pixels.setPixelColor(0, pixels.Color(150, 0, 0));
    pixels.show();  
    delay(500); 
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();  
    delay(500); 
 
  logger.println("Adafruit Serial Flash read write example");
  logger.print("JEDEC ID: "); logger.println(flash.getJEDECID(), HEX);
  logger.print("Flash size: "); logger.println(flash.size());
  Serial.flush();
 
  logger.println();
  // First call begin to mount the filesystem.  Check that it returns true
  // to make sure the filesystem was mounted.
  if (!fatfs.begin(&flash)) {
    logger.println("Error, failed to mount newly formatted filesystem!");
    logger.println("Was the flash chip formatted with the SdFat_format example?");
    while(1) yield();
  }
  logger.println("Mounted filesystem!");
  logger.println();
 flash_setup  = true;
 

}


#endif
