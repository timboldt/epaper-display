# E-Paper Display

## Disclaimer

This is not an officially supported Google product.

## Introduction

This project is an experiment to make a low-power desktop clock, using a Waveshare EPD display.

## Hardware

I chose almost all Adafruit parts for this project, because they are well made, hassle free, and plug together like Legos.

* Adafruit Feather M0 Express
* Adafruit DS3231 RTC Featherwing
* Adafruit Airlift Featherwing
* Adafruit RTL5111 Reset Timer
* Waveshare 4.2" E-Paper Module
* BME280 Temperature/Pressure/Humidity Sensor

The end result is a clock that runs for months on a small 1S LiPo battery.

![](assets/clock-front.jpg)

## Building and running

1. Install arduino-cli.
1. Install Adafruit hardware support. (Add https://www.adafruit.com/package_adafruit_index.json to the "additional URLs".)
1. Install all the necessary libraries:
```
arduino-cli core install adafruit:samd
arduino-cli lib install RTClib
arduino-cli lib install "Adafruit GFX Library"
arduino-cli lib install "GxEPD2"
arduino-cli lib install ArduinoHttpClient
arduino-cli lib install BMx280MI
arduino-cli lib install ArduinoJson
arduino-cli lib install "Adafruit SPIFlash"
arduino-cli lib install "Arduino Low Power"
```
1. Compile and flash with `arduino-cli compile && arduino-cli upload -p /dev/ttyACM0`.
