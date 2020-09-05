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


def draw_gauge(img, x, y, radius, lbl, rawval, minval, maxval):
    val = rawval
    val = min(maxval, max(minval, val))
    val = (val - minval) / (maxval - minval) - 0.5

    draw = ImageDraw.Draw(img)

    # Gauge Face
    draw.arc(
        [
            (x - radius, y-radius),
            (x+radius, y+radius),
        ],
        start=150,
        end=30,
        fill=FOREGROUND_COLOR,
        width=1)
    for i in range(-5, 6, 1):
        dx = math.sin(i * 2 * math.pi / 16)
        dy = math.cos(i * 2 * math.pi / 16)
        dot_size = 3 if i % 5 == 0 else 1
        draw.ellipse(
            [
                (x + 0.8*radius * dx - dot_size, y - 0.8*radius * dy - dot_size),
                (x + 0.8*radius * dx + dot_size, y - 0.8*radius * dy + dot_size),
            ],
            outline=FOREGROUND_COLOR,
            width=1
        )

    # Gauge needle
    gauge_angle = val * 10 / 16 * 2 * math.pi
    mx = math.sin(gauge_angle)
    my = math.cos(gauge_angle)
    draw.polygon(
        [
            (x + radius*0.9*mx, y - radius*0.9*my),
            (x + radius*0.1*my, y + radius*0.1*mx),
            (x - radius*0.1*mx, y + radius*0.1*my),
            (x - radius*0.1*my, y - radius*0.1*mx),
        ],
        fill=FOREGROUND_COLOR)

#     display.setFont(&Picopixel);
#     int16_t fx, fy;
#     uint16_t fw, fh;
#     display.getTextBounds(label, x, y, &fx, &fy, &fw, &fh);
#     display.setCursor(x - fw / 2, y + gauge_radius - fh / 2);
#     display.setTextColor(GxEPD_BLACK);
#     display.print(label);
#     char buffer[20];
#     if (abs(raw_value) < 100) {
#         sprintf(buffer, "%.2f", raw_value);
#     } else {
#         sprintf(buffer, "%.1f", raw_value);
#     }
#     String value_string(buffer);
#     display.getTextBounds(value_string, x, y, &fx, &fy, &fw, &fh);
#     display.setCursor(x - fw / 2, y + gauge_radius * 0.6 - fh / 2);
#     display.setTextColor(GxEPD_BLACK);
#     display.print(value_string);



def main():
    now = time.localtime()
    img = Image.new("L", (800, 600), color=BACKGROUND_COLOR)
    draw_clock(img, 240, 240, 200, now.tm_hour, now.tm_min)
    draw_gauge(img, 500, 100, 40, "Test 0", 40, 0, 100)
    draw_gauge(img, 500, 200, 40, "Test 50", 52, 0, 100)
    draw_gauge(img, 500, 300, 40, "Test 100", 66, 0, 100)
    img = img.resize((400, 300), resample=Image.BOX)
    img = img.quantize(4)
    img.show()


if __name__ == "__main__":
    main()
