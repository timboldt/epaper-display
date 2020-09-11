#  Copyright 2020 Google LLC
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      https:#www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

import math
import time
from PIL import Image, ImageDraw, ImageFont

BLACK_COLOR = 0x00
DKGREY_COLOR = 0x80
LTGREY_COLOR = 0xC0
WHITE_COLOR = 0xFF


def draw_cache_age(img, age):
    draw = ImageDraw.Draw(img)
    fnt = ImageFont.truetype("Cantarell-Regular.otf", 22)
    draw.text((3, 3), "{}".format(round(age)),
              font=fnt, fill=BLACK_COLOR)


def draw_date(img, dt):
    draw = ImageDraw.Draw(img)
    fnt = ImageFont.truetype("Cantarell-Regular.otf", 40)
    date_string = time.strftime("%a, %b %d", dt)
    (szx, _) = draw.textsize(date_string, font=fnt)
    draw.text((800 - 20 - szx, 10), date_string,
              font=fnt, fill=BLACK_COLOR)


def draw_clock(img, x, y, radius, hour, minute):
    draw = ImageDraw.Draw(img)

    # Draw the clock outline.
    draw.ellipse(
        [
            (x - radius, y-radius),
            (x+radius, y+radius),
        ],
        outline=BLACK_COLOR,
        # width=3)
        width=int(radius/30+0.5))

    # Draw the tick marks.
    for i in range(12):
        dx = math.sin(i / 12 * 2 * math.pi)
        dy = math.cos(i / 12 * 2 * math.pi)
        draw.line(
            [
                (int(x + radius*0.85*dx), int(y - radius*0.85*dy)),
                (int(x + radius*0.95*dx), int(y - radius*0.95*dy)),
            ],
            fill=BLACK_COLOR,
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
        fill=LTGREY_COLOR,
        outline=BLACK_COLOR)

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
        # outline=WHITE_COLOR,
        fill=BLACK_COLOR)

    # Draw the pin in the center.
    pin_radius = int(radius*0.02)
    draw.ellipse(
        [
            (x - pin_radius, y-pin_radius),
            (x+pin_radius, y+pin_radius),
        ],
        fill=DKGREY_COLOR,
        outline=WHITE_COLOR,
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
        fill=BLACK_COLOR,
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
            outline=BLACK_COLOR,
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
        fill=BLACK_COLOR)

    fnt = ImageFont.truetype("Cantarell-Regular.otf", 20)
    (szx, szy) = draw.textsize(lbl, font=fnt)
    draw.text((x - szx/2, y + radius * 1.05 - szy), lbl,
              font=fnt, fill=BLACK_COLOR)

    if abs(rawval) < 100:
        valtxt = "{:.1f}".format(rawval)
    else:
        valtxt = "{:d}".format(round(rawval))
    fnt = ImageFont.truetype("Cantarell-Regular.otf", 16)
    (szx, szy) = draw.textsize(valtxt, font=fnt)
    draw.text((x - szx/2, y + radius * 0.45 - szy),
              valtxt, font=fnt, fill=BLACK_COLOR)


def draw_stoplight(img, x, y, radius, on):
    draw = ImageDraw.Draw(img)
    draw.ellipse(
        [
            (x - radius, y-radius),
            (x+radius, y+radius),
        ],
        fill=DKGREY_COLOR if on else WHITE_COLOR,
        outline=BLACK_COLOR,
        width=2)

    draw.ellipse(
        [
            (x + radius*0.2, y-radius*0.4),
            (x+radius*0.4, y-radius*-0.1),
        ],
        fill=WHITE_COLOR)


def draw_chart(img, x, y, radius, label, vals, prev):
    vals = [prev] + vals
    mn = min(vals) * 0.998
    mx = max(vals) * 1.002
    if mx == mn:
        mx += 1e-6
    draw = ImageDraw.Draw(img)
    left = x - radius
    top = y - radius

    if prev != 0:
        valtxt = "{} {:+.1f}%".format(label, (vals[-1] - prev) / prev * 100)
    else:
        valtxt = label

    fnt = ImageFont.truetype("Cantarell-Regular.otf", 20)
    (szx, _) = draw.textsize(valtxt, font=fnt)
    (_, szy) = draw.textsize("Gg!", font=fnt)
    draw.text((x - szx/2, y + radius - szy/2),
              valtxt, font=fnt, fill=BLACK_COLOR)
    height = radius * 2 - szy/2
    width = radius * 2

    if len(vals) > 1:
        points = []
        px = left + 2
        for val in vals:
            py = top + height - height * (val - mn) / (mx - mn)
            points.append((px, py))
            px += (width-2)/len(vals)
        draw.line(points, fill=BLACK_COLOR, width=1)

    draw.rectangle(
        [
            (left, top),
            (left+width, top + height),
        ],
        outline=BLACK_COLOR,
        width=2)


def image_correction(x):
    if x == WHITE_COLOR:
        return x
    if x < DKGREY_COLOR:
        return BLACK_COLOR
    if x <= LTGREY_COLOR:
        return DKGREY_COLOR
    return LTGREY_COLOR
