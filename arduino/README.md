# E-Paper Display

## Disclaimer

This is not an officially supported Google product.

## Introduction

This project was an experiment to make a low-power desktop clock, using an e-paper display.

The low-power thing worked out pretty well. Here is the power consumption, on each update, which happens every ~180 seconds:

![](assets/power-consumption.png)

## Software

The original version used TinyGo (see the v1 folder). I then re-wrote it as conventional Arduino firmware.

The main file is `epaper-display.ino`. Sub-modules are in their own `.ino` files, which the Arduino compiler merges together.

There is also a `global.h` which allowed me to satisfy VSCode's need to have an include file in every `.ino` file.

P.S. The code quality of this project is not that high. Most of the code is C-like, and the networking logic does not have very good separation of concerns or error handling flow. On the other hand it was a one-off project and it is successfully serving its intended purpose on my desk.

## Hardware

I chose almost all Adafruit parts for this project, because they are well made, hassle free, and plug together like Legos.

* Adafruit Feather M0 Express
* Adafruit DS3231 RTC Featherwing
* Adafruit Airlift Featherwing
* Adafruit RTL5111 Reset Timer
* Waveshare 4.2" E-Paper Module
* BME280 Temperature/Pressure/Humidity Sensor

The end result is a clock that runs for months on a small LiPo battery.

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
