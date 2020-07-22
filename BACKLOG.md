## Backlog of Stuff to Do

### v0.1 MVP (A clock that runs on USB power with battery backup.)

* [X] Get it to flash on the new Feather M0.
* [X] Get the RTC clock working over I2C. (And set it to the right time, initially.)
* [X] Show the time with a simple OLED display.

### v0.2 Show the time on an e-paper display.

* [ ] Extend the existing 2.13" driver to work with a 4" EPD.
* [ ] Explore possibility of doing 2-bit grey scale without a 30KB buffer.
* [ ] Add TPL5110 Low Power Timer, so that everything can run on batteries for a long time.

### v0.3 Add network capability

* [ ] Add support for WiFiNINA (ESP32 over SPI).
* [ ] Occasionally refresh the time using NTP.

### v0.4 Add interesting information

* Weather
* Sunrise/sunset
* Stock market
* etc.

### Misc build-related stuff

* Get Go Modules working when TinyGo releases the May 2020 changes in the next release.