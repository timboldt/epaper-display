#pragma once

#include "Adafruit_FRAM_I2C.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SPIFlash.h>
#include <Arduino.h>
//#include <ArduinoLowPower.h>
#include <ArduinoJson.h>
#include <BMx280I2C.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/Picopixel.h>
#include <GxEPD2_BW.h>
#include <HttpClient.h>
#include <RTClib.h>
#include <SdFat.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>

#include "wifi_secrets.h"

//==================
// Pin definitions.
//==================

// Waveshare EPD.
#define EPD_BUSY A2
#define EPD_RESET A3
#define EPD_DC A4
#define EPD_CS A5

// TPL5111.
#define POWER_OFF 5

// Voltage divider for LiPo.
//#define VBATPIN 9
#define VBATPIN A6

// Adafruit ESP32 Airlift.
#define SPIWIFI SPI      // The SPI port
#define SPIWIFI_SS 13    // Chip select pin
#define ESP32_RESETN 12  // Reset pin
#define SPIWIFI_ACK 11   // a.k.a BUSY or READY pin
#define ESP32_GPIO0 -1

//==================
// Misc. Consts
//==================

#define MAX_DISPLAY_BUFFER_SIZE 15000ul  // ~15k is a good compromise
#define MAX_HEIGHT(EPD)                                        \
    (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) \
         ? EPD::HEIGHT                                         \
         : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))

// Cache offsets.
#define CACHEKEY_IS_VALID 0
#define CACHEKEY_OUTSIDE_TEMPERATURE 1
#define CACHEKEY_OUTSIDE_TEMPERATURE 2
#define CACHEKEY_OUTSIDE_HUMIDITY 3
#define CACHEKEY_AQI_OZONE 4
#define CACHEKEY_AQI_PM25 5
#define CACHEKEY_MARKETS_BTC 6
#define CACHEKEY_MARKETS_GOOG 7
#define CACHEKEY_MARKETS_GOOG_PCT 8
#define CACHEKEY_MARKETS_SP500 9
#define CACHEKEY_MARKETS_SP500_PCT 10
#define CACHEKEY_LAST_NET 11
//
#define CACHEKEY_NUM_KEYS 64

//==================
// Devices.
//==================

// Waveshare 4.2" B/W EPD.
GxEPD2_BW<GxEPD2_420, MAX_HEIGHT(GxEPD2_420)> display(GxEPD2_420(EPD_CS, EPD_DC,
                                                                 EPD_RESET,
                                                                 EPD_BUSY));


                                                                 //#include <Adafruit_SGP30.h>

// SGP30 air quality sensor.
// NOT INSTALLED: Adafruit_SGP30 sgp;

// BME280 temperature, humidity and pressure.
BMx280I2C bmx280(0x76);

// DS3231 Real-Time Clock.
RTC_DS3231 rtc;

// Adafruit FRAM
Adafruit_FRAM_I2C fram;

// On-board Flash Storage.
#if defined(EXTERNAL_FLASH_USE_QSPI)
  Adafruit_FlashTransport_QSPI flashTransport;

#elif defined(EXTERNAL_FLASH_USE_SPI)
  Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);

#else
  #error No QSPI/SPI flash are defined on your board variant.h !
#endif
Adafruit_SPIFlash flash(&flashTransport);
FatFileSystem fatfs;

// Adafruit ESP32 Airlift.
WiFiClient wifi;
WiFiSSLClient wifissl;