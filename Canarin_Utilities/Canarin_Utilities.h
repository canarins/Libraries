/**
 * @file      utilities.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2022-09-16
 *
 */

#ifndef CANARIN_UTILS_H_
#define CANARIN_UTILS_H_

#include "arduino.h"
#include <string.h>

#include <Arduino.h>

#pragma once

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
// #define LILYGO_ESP32S3_CAM_PIR_VOICE // Has PSRAM
#define LILYGO_ESP32S3_CAM_SIM7080G // Has PSRAM

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Concatenate "Pico+" with the __DATE__ macro
#define VERSION "Pico_" TOSTRING(__DATE__)

char version[20] = VERSION;



// Set this to true if using AP mode
#define USING_AP_MODE true

// ===================
// Pins
// ===================
#ifdef I2C_SDA
#undef I2C_SDA
#endif

#ifdef I2C_SCL
#undef I2C_SCL
#endif

#if defined(LILYGO_ESP32S3_CAM_PIR_VOICE)

#define PWDN_GPIO_NUM (-1)
#define RESET_GPIO_NUM (17)
#define XCLK_GPIO_NUM (38)
#define SIOD_GPIO_NUM (5)
#define SIOC_GPIO_NUM (4)
#define VSYNC_GPIO_NUM (8)
#define HREF_GPIO_NUM (18)
#define PCLK_GPIO_NUM (12)
#define Y9_GPIO_NUM (9)
#define Y8_GPIO_NUM (10)
#define Y7_GPIO_NUM (11)
#define Y6_GPIO_NUM (13)
#define Y5_GPIO_NUM (21)
#define Y4_GPIO_NUM (48)
#define Y3_GPIO_NUM (47)
#define Y2_GPIO_NUM (14)

#define I2C_SDA (7)
#define I2C_SCL (6)

#define PIR_INPUT_PIN (39)
#define PMU_INPUT_PIN (2)

#define IIS_WS_PIN (42)
#define IIS_DIN_PIN (41)
#define IIS_SCLK_PIN (40)

#define BUTTON_CONUT (1)
#define USER_BUTTON_PIN (0)
#define BUTTON_ARRAY \
  {                  \
    USER_BUTTON_PIN  \
  }

#define BOARD_CAM_1V8_CHANNEL 1
#define BOARD_CAM_3V0_CHANNEL 1
#define BOARD_CAM_2V8_CHANNEL 1
#define BOARD_MIC_CHANNEL 2
#define BOARD_PIR_CHANNEL 2

#define USING_MICROPHONE

#elif defined(LILYGO_ESP32S3_CAM_SIM7080G)

#define PWDN_GPIO_NUM (-1)
#define RESET_GPIO_NUM (18)
#define XCLK_GPIO_NUM (8)
#define SIOD_GPIO_NUM (2)
#define SIOC_GPIO_NUM (1)
#define VSYNC_GPIO_NUM (16)
#define HREF_GPIO_NUM (17)
#define PCLK_GPIO_NUM (12)
#define Y9_GPIO_NUM (9)
#define Y8_GPIO_NUM (10)
#define Y7_GPIO_NUM (11)
#define Y6_GPIO_NUM (13)
#define Y5_GPIO_NUM (21)
#define Y4_GPIO_NUM (48)
#define Y3_GPIO_NUM (47)
#define Y2_GPIO_NUM (14)

#define I2C_SDA (15)
#define I2C_SCL (7)

#define PMU_INPUT_PIN (6)

#define BUTTON_CONUT (1)
#define USER_BUTTON_PIN (0)
#define BUTTON_ARRAY \
  {                  \
    USER_BUTTON_PIN  \
  }

#define BOARD_MODEM_PWR_PIN (41)
#define BOARD_MODEM_DTR_PIN (42)
#define BOARD_MODEM_RI_PIN (3)
#define BOARD_MODEM_RXD_PIN (4)
#define BOARD_MODEM_TXD_PIN (5)

#define USING_MODEM

#define SDMMC_CMD (39)
#define SDMMC_CLK (38)
#define SDMMC_DATA (40)
#else
#error "Camera model not selected"
#endif

#include <Canarin_Logger.h>

#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h"

// #include <SoftwareSerial.h>
// SoftwareSerial pmSerial(44, 43);

#define I2C_SDA 15
#define I2C_SCL 7

#define I2C_SDA_2 15
#define I2C_SCL_2 7

#define I2C_SDA 13 // TODO Remember
#define I2C_SCL 21

#define CS_MPU 12
#define CS_MEM 8
#define SET_PMS 3
#define PIN_RGB 16

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(1, PIN_RGB, NEO_GRB + NEO_KHZ800);

float lat2 = 0;
float lon2 = 0;
float speed2 = 0;
float alt2 = 0;
int vsat2 = 0;
int usat2 = 0;
float accuracy2 = 0;
int year2 = 0;
int month2 = 0;
int day2 = 0;
int hour2 = 0;
int min2 = 0;
int sec2 = 0;
bool level = false;

// TODO What is that and where should it go
#include <SPI.h>
#include <chrono>
#include <Canarin_timeout.h>

#include <Canarin_Configuration.h>

void Canarin_Utilities_setup(){

String processedVersion(version);




processedVersion.replace("\"", ""); // Remove '"'
processedVersion.replace(" ", "_"); // Replace ' ' with '_'

processedVersion.toCharArray(version, 20);

logger.println(version);

}




#endif /* CANARIN_UTILS_H_ */