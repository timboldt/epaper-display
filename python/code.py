import analogio
import board
import busio
from digitalio import DigitalInOut, Direction
import displayio
import terminalio
import time

from adafruit_display_text import label
import adafruit_bme280
import adafruit_ds3231
import adafruit_il0398

BLACK = 0x000000
WHITE = 0xFFFFFF

FOREGROUND_COLOR = BLACK
BACKGROUND_COLOR = WHITE

DISPLAY_WIDTH = 400
DISPLAY_HEIGHT = 300

tpl5111_done = DigitalInOut(board.D5)
tpl5111_done.direction = Direction.OUTPUT
tpl5111_done.value = False

i2c = busio.I2C(board.SCL, board.SDA)
rtc = adafruit_ds3231.DS3231(i2c)
bme280 = adafruit_bme280.Adafruit_BME280_I2C(i2c, address=0x76)
bme280.sea_level_pressure = 1013.25
vbat_voltage = analogio.AnalogIn(board.VOLTAGE_MONITOR)

displayio.release_displays()
spi = board.SPI() # Uses SCK and MOSI
epd_cs = board.A5
epd_dc = board.A4
epd_reset = board.A3
epd_busy = board.A2
display_bus = displayio.FourWire(spi, command=epd_dc, chip_select=epd_cs, reset=epd_reset,
                                 baudrate=1000000)
time.sleep(1)
display = adafruit_il0398.IL0398(display_bus, width=DISPLAY_WIDTH, height=DISPLAY_HEIGHT, seconds_per_frame=20,
                                 busy_pin=epd_busy)
background_bitmap = displayio.Bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT, 1)
palette = displayio.Palette(1)
palette[0] = BACKGROUND_COLOR

while True:
    print("\nTemperature: %0.1f C" % bme280.temperature)
    print("Humidity: %0.1f %%" % bme280.humidity)
    print("Pressure: %0.1f hPa" % bme280.pressure)
    print("Altitude = %0.2f meters" % bme280.altitude)

    temperature = rtc.temperature
    print("Temperature: {:.2f}".format(temperature))
    voltage = (vbat_voltage.value * 3.3) / 65536 * 2
    print("Voltage: {:.2f}".format(voltage))

    t = rtc.datetime
    print(
        "The date is {}/{}/{}".format(
            t.tm_year, t.tm_mon, t.tm_mday
        )
    )
    print("The time is {}:{:02}:{:02}".format(t.tm_hour, t.tm_min, t.tm_sec))

    g = displayio.Group(max_size=10)
    bg = displayio.TileGrid(background_bitmap, pixel_shader=palette)
    g.append(bg)

    text_group = displayio.Group(max_size=10, scale=2, x=20, y=150)
    text = '\n'.join([
        "The time is {}:{:02}:{:02}".format(t.tm_hour, t.tm_min, t.tm_sec),
        "Temperature: {:.2f}".format(bme280.temperature),
        "Humidity: {:0.1f} %".format(bme280.humidity),
        "Pressure: {:0.1f} hPa".format(bme280.pressure),
        "Voltage: {:.2f}".format(voltage),
        "The date is {}/{}/{}".format(
            t.tm_year, t.tm_mon, t.tm_mday
        )])
    text_area = label.Label(terminalio.FONT, text=text, color=FOREGROUND_COLOR)
    text_group.append(text_area)
    g.append(text_group)

    time.sleep(1)

    display.show(g)
    display.refresh()

    time.sleep(5)
    tpl5111_done.value = True

    time.sleep(180)