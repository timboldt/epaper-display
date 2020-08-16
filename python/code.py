import board
import busio
import displayio
from digitalio import DigitalInOut
import microcontroller
import supervisor
import terminalio
import time

from adafruit_display_text import label
import adafruit_bme280
import adafruit_ds3231

import eclockhw
import eclocknet
import eclockui

ALTITUDE_ADJUSTMENT = 10.5

tpl5111 = eclockhw.PowerSwitch(board.D5)
i2c = busio.I2C(board.SCL, board.SDA)
rtc = adafruit_ds3231.DS3231(i2c)
bme280 = adafruit_bme280.Adafruit_BME280_I2C(i2c, address=0x76)
battery = eclockhw.Battery(board.VOLTAGE_MONITOR)
display = eclockui.Display()
net = eclocknet.NetCache(
    cs=DigitalInOut(board.D13),
    ready=DigitalInOut(board.D11),
    reset=DigitalInOut(board.D12))

if net.seconds_since_last_refresh(rtc) > 60:
    net.refresh(rtc)

temperature = rtc.temperature
voltage = battery.voltage()
clk = rtc.datetime
if clk.tm_hour < 12:
    am_pm = "am"
else:
    am_pm = "pm"
if clk.tm_hour == 0:
    twelve_hour = 12
elif clk.tm_hour > 12:
    twelve_hour = clk.tm_hour - 12
else:
    twelve_hour = clk.tm_hour

frame = display.new_frame()

display.draw_clock(5, 40, 126, twelve_hour, clk.tm_min)
display.draw_clock(330, 230, 30, (twelve_hour+3) % 12, clk.tm_min)

title_group = displayio.Group(max_size=2, scale=2, x=20, y=20)
title_group.append(
    label.Label(
        terminalio.FONT,
        text="{}/{}/{} {}:{:02} {}".format(
            clk.tm_year,
            clk.tm_mon,
            clk.tm_mday,
            twelve_hour,
            clk.tm_min,
            am_pm),
        color=display.FOREGROUND_COLOR))
frame.append(title_group)

text_group = displayio.Group(max_size=2, scale=1, x=290, y=70)
text = '\n'.join([
    "{}/{}/{}".format(
        clk.tm_year, clk.tm_mon, clk.tm_mday
    ),
    "{}:{:02}:{:02}".format(clk.tm_hour, clk.tm_min, clk.tm_sec),
    "Battery: {:.2f} V".format(voltage),
    "Bitcoin: {:.0f} USD".format(net["btc_usd"]),
    "Clock: {:.2f} C".format(rtc.temperature),
    "Inside: {:.2f} C".format(bme280.temperature),
    "Inside: {:0.1f}%".format(bme280.humidity),
    "Inside: {:0.1f} hPa".format(bme280.pressure + ALTITUDE_ADJUSTMENT),
    "Outside: {:.2f} C".format(net["temperature"]),
    "Outside: {:.1f}%".format(net["humidity"]),
    "Outside: {:.1f} hPa".format(net["pressure"])
])
print(text)
text_group.append(
    label.Label(
        terminalio.FONT,
        text=text,
        color=display.FOREGROUND_COLOR))
frame.append(text_group)

display.show()

tpl5111.turn_off()

time.sleep(180)
supervisor.reload()
