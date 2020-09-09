#!/usr/bin/env python3

#  Copyright 2020 Google LLC
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      https:#www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

import os
import sys
import time

from bme280pi import Sensor
from PIL import Image

import ui
import net
import storage

try:
    libdir = os.path.join(os.path.dirname(
        os.path.realpath(__file__)), 'third_party')
    if os.path.exists(libdir):
        sys.path.append(libdir)
    # Waveshare defs
    from waveshare_epd import epd4in2
except ImportError:
    print("Cannot find WaveShare EPD driver!")
    raise


def read_network():
    if net.time_since_last_fetch() > 12*60:
        net.get_btc_price()
        net.get_weather()
        net.get_air_quality()
        net.get_stock_intraday("DIA")
        net.get_stock_intraday("SPY")
        net.get_stock_intraday("GOOG")
        net.set_last_fetch_time(time.time())


def read_sensors():
    bme = Sensor(address=0x76)
    bme_data = bme.get_data()
    storage.cache["bme_temperature"] = bme_data["temperature"]
    storage.cache["bme_humidity"] = bme_data["humidity"]
    pressure = bme_data["pressure"] + 12
    storage.cache["bme_pressure"] = pressure
    if not "bme_pressure_history" in storage.cache:
        storage.cache["bme_pressure_history"] = []
    storage.cache["bme_pressure_history"].append(pressure)
    storage.cache["bme_pressure_history"] = storage.cache["bme_pressure_history"][-100:]


def update_display():
    now = time.localtime()
    img = Image.new("L", (800, 600), color=ui.WHITE_COLOR)

    ui.draw_cache_age(img, net.time_since_last_fetch())

    ui.draw_date(img, now)
    ui.draw_clock(img, 240, 240, 200, now.tm_hour, now.tm_min)

    ui.draw_gauge(img, 510, 130, 50, "Temp. In",
                  storage.cache["bme_temperature"], 10, 40)
    ui.draw_stoplight(img, 620, 130, 30, storage.cache["temperature"] >
                      25 and storage.cache["temperature"] > storage.cache["bme_temperature"])
    ui.draw_gauge(img, 730, 130, 50, "Temp. Out",
                  storage.cache["temperature"], 10, 40)

    #ui.draw_gauge(img, 510, 260, 50, "Ozone", storage.cache["ozone"], 0, 200)
    ui.draw_chart(img, 510, 260, 50, "AQI",
                  storage.cache["aqi_history"], storage.cache["aqi_history"][0])
    ui.draw_stoplight(img, 620, 260, 30,
                      storage.cache["ozone"] > 100 or storage.cache["pm25"] > 100)
    ui.draw_gauge(img, 730, 260, 50, "PM 2.5", storage.cache["pm25"], 0, 200)

    ui.draw_gauge(img, 510, 390, 50, "Pressure",
                  storage.cache["bme_pressure"], 1000, 1026.5)
    ui.draw_chart(img, 620, 390, 50, "Pressure",
                  storage.cache["bme_pressure_history"], storage.cache["bme_pressure_history"][0])
    ui.draw_gauge(img, 730, 390, 50, "RH% In",
                  storage.cache["bme_humidity"], 0, 100)

    ui.draw_gauge(img, 70, 520, 50, "DJIA",
                  storage.cache["DIA_intraday"][-1]*100, 20000, 30000)
    ui.draw_chart(img, 180, 520, 50, "DJIA",
                  storage.cache["DIA_intraday"], storage.cache["DIA_previous"])

    ui.draw_gauge(img, 290, 520, 50, "Google",
                  storage.cache["GOOG_intraday"][-1], 1000, 2000)
    ui.draw_chart(img, 400, 520, 50, "GOOG",
                  storage.cache["GOOG_intraday"], storage.cache["GOOG_previous"])

    ui.draw_gauge(img, 510, 520, 50, "S&P 500",
                  storage.cache["SPY_intraday"][-1]*10, 2000, 4000)

    ui.draw_gauge(img, 620, 520, 50, "Bitcoin",
                  storage.cache["btc_usd"], 7000, 13000)
    if "btc_history" in storage.cache:
        ui.draw_chart(img, 730, 520, 50, "BTC",
                      storage.cache["btc_history"], storage.cache["btc_history"][0])

    img = img.resize((400, 300), resample=Image.BOX)
    img = Image.eval(img, ui.image_correction)
    # img.show()

    epd = epd4in2.EPD()
    epd.Init_4Gray()
    epd.display_4Gray(epd.getbuffer_4Gray(img))
    epd.sleep()
    # epd.Dev_exit()


def main():
    storage.load_cache()
    read_network()
    read_sensors()
    print(storage.cache)
    update_display()
    storage.save_cache()
    # while True:
    #    read_network()
    #    for _ in range(10):
    #        time.sleep(120)


if __name__ == "__main__":
    main()
