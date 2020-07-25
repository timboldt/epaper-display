"""Simple test script for 4.2" 400x300 black and white displays.

Supported products:
  * WaveShare 4.2" Black and White
    * https://www.waveshare.com/product/modules/oleds-lcds/e-paper/4.2inch-e-paper.htm
    * https://www.waveshare.com/product/modules/oleds-lcds/e-paper/4.2inch-e-paper-module.htm
  """

import time
import board
import displayio
import terminalio
from adafruit_display_text import label
import adafruit_il0398

displayio.release_displays()

spi = board.SPI() # Uses SCK and MOSI
epd_cs = board.D6
epd_dc = board.D10
epd_reset = board.D11
epd_busy = board.D12

display_bus = displayio.FourWire(spi, command=epd_dc, chip_select=epd_cs, reset=epd_reset,
                                 baudrate=1000000)
time.sleep(1)

display = adafruit_il0398.IL0398(display_bus, width=400, height=300, seconds_per_frame=20,
                                 busy_pin=epd_busy)

# Set text, font, and color
text = "HELLO WORLD"
font = terminalio.FONT
color = 0x0000FF

bitmap = displayio.Bitmap(40, 40, 2)
 
palette = displayio.Palette(2)
palette[0] = 0
palette[1] = 0xFFFFFF
 
for x in range(10, 20):
    for y in range(10, 20):
      bitmap[x, y] = 1
 
tile_grid = displayio.TileGrid(bitmap, pixel_shader=palette)
 
group = displayio.Group()
group.append(tile_grid)
display.show(group)
display.refresh()
displayio.release_displays()

time.sleep(120)