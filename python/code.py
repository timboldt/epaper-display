import board
import busio
import displayio
import terminalio
import time

from adafruit_display_text import label
import adafruit_bme280
import adafruit_ds3231

import eclockhw
import eclockui

tpl5111 = eclockhw.PowerSwitch()
i2c = busio.I2C(board.SCL, board.SDA)
rtc = adafruit_ds3231.DS3231(i2c)
bme280 = adafruit_bme280.Adafruit_BME280_I2C(i2c, address=0x76)
bme280.sea_level_pressure = 1013.25
battery = eclockhw.Battery()
display = eclockui.Display()

while True:
    temperature = rtc.temperature
    voltage = battery.voltage()
    clk = rtc.datetime

    frame = display.new_frame()
    text_group = displayio.Group(max_size=10, scale=2, x=20, y=150)
    text = '\n'.join([
        "{}/{}/{}".format(
            clk.tm_year, clk.tm_mon, clk.tm_mday
        ),
        "{}:{:02}:{:02}".format(clk.tm_hour, clk.tm_min, clk.tm_sec),
        "Temperature: {:.2f} C".format(bme280.temperature),
        "Humidity: {:0.1f}%".format(bme280.humidity),
        "Pressure: {:0.1f} hPa".format(bme280.pressure),
        "Battery: {:.2f} V".format(voltage),
    ])
    print(text)
    text_area = label.Label(terminalio.FONT, text=text, color=display.FOREGROUND_COLOR)
    text_group.append(text_area)
    frame.append(text_group)
    display.show()

    tpl5111.turn_off()

    time.sleep(180)