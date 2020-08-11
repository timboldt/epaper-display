# E-Paper Display

## Disclaimer

This is not an officially supported Google product.

## Introduction

This project was an experiment to make a low-power desktop clock, using an e-paper display.

The low-power thing worked out pretty well. Here is the power consumption, on each update, which happens every ~180 seconds:

![](assets/power-consumption.png)

## Software

This clock was my opportunity to experiment with a few different technologies.

The original version used TinyGo is in the `go/` subdirectory.

The second version, in the `arduino/` subdirectory was written using conventional Arduino technologies.

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
