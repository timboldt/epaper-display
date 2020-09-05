#!/usr/bin/env python3

import time
from PIL import Image

import net
import ui


def main():
    #net.get_net_time()
    net.get_btc_price()
    net.get_weather()
    print(net.cache)

    now = time.localtime()
    img = Image.new("L", (800, 600), color=ui.WHITE_COLOR)
    
    ui.draw_date(img, now)
    ui.draw_clock(img, 240, 240, 200, now.tm_hour, now.tm_min)

    ui.draw_gauge(img, 510, 130, 50, "Inside Temp.", 23, 10, 40)
    ui.draw_stoplight(img, 620, 130, 30, True)
    ui.draw_gauge(img, 730, 130, 50, "Outside Temp.", net.cache["temperature"], 10, 40)

    ui.draw_gauge(img, 510, 260, 50, "Bitcoin", net.cache["btc_usd"], 7000, 13000)
    ui.draw_stoplight(img, 620, 260, 30, False)

    ui.draw_gauge(img, 510, 390, 50, "Test 66", 66, 0, 100)
    img = img.resize((400, 300), resample=Image.BOX)
    img = img.quantize(4)
    img.show()


if __name__ == "__main__":
    main()
