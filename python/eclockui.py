import board
import displayio
import terminalio
import time

import adafruit_il0398
from adafruit_display_shapes.rect import Rect

class Display():
    def __init__(self):
        BLACK = 0x000000
        WHITE = 0xFFFFFF

        self.FOREGROUND_COLOR = BLACK
        self.BACKGROUND_COLOR = WHITE

        self.DISPLAY_WIDTH = 400
        self.DISPLAY_HEIGHT = 300

        displayio.release_displays()

        spi = board.SPI() # Uses SCK and MOSI
        epd_cs = board.A5
        epd_dc = board.A4
        epd_reset = board.A3
        epd_busy = board.A2
        display_bus = displayio.FourWire(spi, command=epd_dc, chip_select=epd_cs, reset=epd_reset,
                                        baudrate=1000000)
        time.sleep(1)
        self._display = adafruit_il0398.IL0398(
            display_bus,
            width=self.DISPLAY_WIDTH,
            height=self.DISPLAY_HEIGHT,
            seconds_per_frame=20,
            busy_pin=epd_busy)

    def new_frame(self):
        self._frame = displayio.Group(max_size=10)
        rect = Rect(0, 0, self.DISPLAY_WIDTH, self.DISPLAY_HEIGHT, fill=self.BACKGROUND_COLOR)
        self._frame.append(rect)
        return self._frame

    def show(self):
        self._display.show(self._frame)
        self._display.refresh()
        # TODO: Detect when EPD is done refreshing.
        time.sleep(5)




