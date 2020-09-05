#!/usr/bin/env python3

import os
import sys
import time

from PIL import Image

import net
import ui

libdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'third_party')
print(libdir)
if os.path.exists(libdir):
    sys.path.append(libdir)
from waveshare_epd import epd4in2

def main():
    #net.get_net_time()
    net.get_btc_price()
    net.get_weather()
    net.get_air_quality()

    net.cache["bme_temperature"] = 23
    net.cache["bme_humidity"] = 50

    print(net.cache)

    now = time.localtime()
    img = Image.new("L", (800, 600), color=ui.WHITE_COLOR)
    
    ui.draw_date(img, now)
    ui.draw_clock(img, 240, 240, 200, now.tm_hour, now.tm_min)

    ui.draw_gauge(img, 510, 130, 50, "Temp. In", net.cache["bme_temperature"], 10, 40)
    ui.draw_stoplight(img, 620, 130, 30, net.cache["temperature"] > 25 and net.cache["temperature"] > net.cache["bme_temperature"])
    ui.draw_gauge(img, 730, 130, 50, "Temp. Out", net.cache["temperature"], 10, 40)

    ui.draw_gauge(img, 510, 260, 50, "Ozone", net.cache["ozone"], 0, 200)
    ui.draw_stoplight(img, 620, 260, 30, net.cache["ozone"] > 100 or net.cache["pm25"] > 100)
    ui.draw_gauge(img, 730, 260, 50, "PM 2.5", net.cache["pm25"], 0, 200)

    ui.draw_gauge(img, 510, 390, 50, "Pressure", net.cache["pressure"], 1000, 1026.5)
    ui.draw_gauge(img, 620, 390, 50, "RH% In", net.cache["bme_humidity"], 0, 100)
    ui.draw_gauge(img, 730, 390, 50, "RH% Out", net.cache["humidity"], 0, 100)

    ui.draw_gauge(img, 620, 520, 50, "Bitcoin", net.cache["btc_usd"], 7000, 13000)
    ui.draw_gauge(img, 730, 520, 50, "Bitcoin", net.cache["btc_usd"], 7000, 13000)

    ui.draw_stockchart(img)

    img = img.resize((400, 300), resample=Image.BOX)
    img = Image.eval(img, ui.image_correction) 
    #img.show()

    epd = epd4in2.EPD()
    epd.Init_4Gray()
    epd.display_4Gray(epd.getbuffer_4Gray(img))
    epd.sleep()
    #epd.Dev_exit()

if __name__ == "__main__":
    main()
