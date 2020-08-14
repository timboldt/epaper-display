import board
import displayio
import terminalio
import time

import adafruit_il0398

class Display():
    def __init__(self):
        BLACK = 0x000000
        WHITE = 0xFFFFFF

        self.FOREGROUND_COLOR = BLACK
        self.BACKGROUND_COLOR = WHITE

        DISPLAY_WIDTH = 400
        DISPLAY_HEIGHT = 300

        displayio.release_displays()

        spi = board.SPI() # Uses SCK and MOSI
        epd_cs = board.A5
        epd_dc = board.A4
        epd_reset = board.A3
        epd_busy = board.A2
        display_bus = displayio.FourWire(spi, command=epd_dc, chip_select=epd_cs, reset=epd_reset,
                                        baudrate=1000000)
        time.sleep(1)
        self._display = adafruit_il0398.IL0398(display_bus, width=DISPLAY_WIDTH, height=DISPLAY_HEIGHT, seconds_per_frame=20,
                                        busy_pin=epd_busy)
        # TODO: This seems inefficient.
        self._background_bitmap = displayio.Bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT, 1)
        self._palette = displayio.Palette(1)
        self._palette[0] = self.BACKGROUND_COLOR

    def new_frame(self):
        self._frame = displayio.Group(max_size=10)
        # TODO: See if a plain rectangle would work instead.
        bg = displayio.TileGrid(self._background_bitmap, pixel_shader=self._palette)
        self._frame.append(bg)
        return self._frame

    def show(self):
        self._display.show(self._frame)
        self._display.refresh()
        # TODO: Detect when EPD is done refreshing.
        time.sleep(5)




