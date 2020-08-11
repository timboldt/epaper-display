# E-Paper Display

## Introduction

This version of the firmware was written using conventional Arduino techniques.

The main file is `arduino.ino`. Sub-modules are in their own `.ino` files, which the Arduino compiler merges together.

There is also a `global.h` which allowed me to satisfy VSCode's need to have an include file in every `.ino` file.

Caveats. The code quality of this project is not that high. Most of the code is C-like, and the networking logic does not have very good separation of concerns or error handling flow. On the other hand it was a one-off project and it is successfully serving its intended purpose on my desk.

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
