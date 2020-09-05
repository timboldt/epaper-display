#!/usr/bin/env python3

import math
import time
from PIL import Image, ImageDraw

FOREGROUND_COLOR = 0
MEDIUM_COLOR = 128+63
BACKGROUND_COLOR = 255


def draw_clock(img, x, y, radius, hour, minute):
    draw = ImageDraw.Draw(img)

    # Draw the clock outline.
    draw.ellipse(
        [
            (x - radius, y-radius),
            (x+radius, y+radius),
        ],
        outline=FOREGROUND_COLOR,
        width=1)
    # width=int(radius/30+0.5))

    # Draw the tick marks.
    for i in range(12):
        dx = math.sin(i / 12 * 2 * math.pi)
        dy = math.cos(i / 12 * 2 * math.pi)
        draw.line(
            [
                (int(x + radius*0.85*dx), int(y - radius*0.85*dy)),
                (int(x + radius*0.95*dx), int(y - radius*0.95*dy)),
            ],
            fill=FOREGROUND_COLOR,
            width=1)

    # Draw the hour hand.
    hour_angle = (hour * 60 + minute) / 60 / 12 * 2 * math.pi
    hx = math.sin(hour_angle)
    hy = math.cos(hour_angle)
    draw.polygon(
        [
                (int(x + radius*0.66*hx), int(y - radius*0.66*hy)),
                (int(x + radius*0.07*hy), int(y + radius*0.07*hx)),
                (int(x - radius*0.15*hx), int(y + radius*0.15*hy)),
                (int(x - radius*0.07*hy), int(y - radius*0.07*hx)),
        ],
        fill=MEDIUM_COLOR,
        outline=FOREGROUND_COLOR)

    # Draw the minute hand.
    minute_angle = minute / 60 * 2 * math.pi
    mx = math.sin(minute_angle)
    my = math.cos(minute_angle)
    draw.polygon(
        [
            (int(x + radius*0.92*mx), int(y - radius*0.92*my)),
            (int(x + radius*0.05*my), int(y + radius*0.05*mx)),
            (int(x - radius*0.15*mx), int(y + radius*0.15*my)),
            (int(x - radius*0.05*my), int(y - radius*0.05*mx)),
        ],
        # outline=BACKGROUND_COLOR,
        fill=FOREGROUND_COLOR)

    # Draw the pin in the center.
    pin_radius = int(radius*0.02)
    draw.ellipse(
        [
            (x - pin_radius, y-pin_radius),
            (x+pin_radius, y+pin_radius),
        ],
        fill=BACKGROUND_COLOR,
        outline=FOREGROUND_COLOR,
        width=1)


def main():
    now = time.localtime()
    im = Image.new("L", (800, 600), color=BACKGROUND_COLOR)
    print(im.format, im.size, im.mode)
    draw_clock(im, 240, 240, 200, now.tm_hour, now.tm_min)
    im = im.resize((400, 300), resample=Image.BILINEAR)
    im.show()


if __name__ == "__main__":
    main()
