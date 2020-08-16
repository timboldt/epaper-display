import board
import displayio
import math
import terminalio
import time

import adafruit_il0398
from adafruit_display_shapes.rect import Rect
from adafruit_display_shapes.circle import Circle
from adafruit_display_shapes.triangle import Triangle
from adafruit_display_shapes.line import Line
from adafruit_display_shapes.polygon import Polygon
from adafruit_display_text import label


class Display():
    def __init__(self):
        BLACK = 0x000000
        WHITE = 0xFFFFFF

        self.FOREGROUND_COLOR = BLACK
        self.BACKGROUND_COLOR = WHITE

        self.DISPLAY_WIDTH = 400
        self.DISPLAY_HEIGHT = 300

        displayio.release_displays()

        spi = board.SPI()  # Uses SCK and MOSI
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
        rect = Rect(0, 0, self.DISPLAY_WIDTH, self.DISPLAY_HEIGHT,
                    fill=self.BACKGROUND_COLOR)
        self._frame.append(rect)
        return self._frame

    def show(self):
        self._display.show(self._frame)
        self._display.refresh()
        # TODO: Detect when EPD is done refreshing.
        time.sleep(5)

    def draw_date(self, x, y, font, dt):
        mon = ["Jan", "Feb", "Mar", "Apr", "May","Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
        dow = ["Monday", "Tuesday", "wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
        g = displayio.Group(max_size=1, scale=2, x=x, y=y)
        g.append(
            label.Label(
                font,
                text="{} {} {}".format(
                    dow[dt.tm_wday],
                    mon[dt.tm_mon - 1],
                    dt.tm_mday),
                color=self.FOREGROUND_COLOR))
        self._frame.append(g)

    def draw_clock(self, x, y, radius, hour, minute):
        grp = displayio.Group(max_size=20, x=x, y=y)

        # Draw the clock outline.
        grp.append(
            Circle(
                radius, radius,
                radius,
                outline=self.FOREGROUND_COLOR,
                stroke=int(radius/30)))

        # Draw the tick marks.
        for i in range(12):
            dx = math.sin(i / 12 * 2 * math.pi)
            dy = math.cos(i / 12 * 2 * math.pi)
            grp.append(
                Line(
                    int(radius + radius*0.85*dx),
                    int(radius - radius*0.85*dy),
                    int(radius + radius*0.95*dx),
                    int(radius - radius*0.95*dy),
                    self.FOREGROUND_COLOR)
            )

        # Draw the hour hand.
        hour_angle = (hour * 60 + minute) / 60 / 12 * 2 * math.pi
        hx = math.sin(hour_angle)
        hy = math.cos(hour_angle)
        grp.append(
            Polygon(
                [
                    (int(radius + radius*0.66*hx), int(radius - radius*0.66*hy)),
                    (int(radius + radius*0.07*hy), int(radius + radius*0.07*hx)),
                    (int(radius - radius*0.15*hx), int(radius + radius*0.15*hy)),
                    (int(radius - radius*0.07*hy), int(radius - radius*0.07*hx)),
                ],
                outline=self.FOREGROUND_COLOR)
        )

        # Draw the minute hand.
        minute_angle = minute / 60 * 2 * math.pi
        mx = math.sin(minute_angle)
        my = math.cos(minute_angle)
        grp.append(
            Triangle(
                int(radius + radius*0.92*mx), int(radius - radius*0.92*my),
                int(radius + radius*0.05*my), int(radius + radius*0.05*mx),
                int(radius - radius*0.05*my), int(radius - radius*0.05*mx),
                fill=self.FOREGROUND_COLOR)
        )
        grp.append(
            Triangle(
                int(radius - radius*0.15*mx), int(radius + radius*0.15*my),
                int(radius + radius*0.05*my), int(radius + radius*0.05*mx),
                int(radius - radius*0.05*my), int(radius - radius*0.05*mx),
                fill=self.FOREGROUND_COLOR)
        )

        # Draw the pin in the center.
        grp.append(
            Circle(
                radius, radius,
                int(radius*0.03),
                fill=self.BACKGROUND_COLOR,
                outline=self.FOREGROUND_COLOR,
                stroke=1))

        self._frame.append(grp)
