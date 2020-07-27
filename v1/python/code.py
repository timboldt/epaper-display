import analogio
import board
import busio
import time

import adafruit_ds3231

i2c = busio.I2C(board.SCL, board.SDA)
rtc = adafruit_ds3231.DS3231(i2c)

vbat_pin = analogio.AnalogIn(board.D9)

days = ("Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday")
while True:
    temperature = rtc.temperature
    print("Temperature: {:.2f}".format(temperature))
    voltage = (vbat_pin.value * 3.3) / 65536 * 2
    print("Voltage: {:.2f}".format(voltage))

    t = rtc.datetime
    print(
        "The date is {} {}/{}/{}".format(
            days[int(t.tm_wday)], t.tm_year, t.tm_mon, t.tm_mday
        )
    )
    print("The time is {}:{:02}:{:02}".format(t.tm_hour, t.tm_min, t.tm_sec))
    time.sleep(1)