package main

import (
	"machine"

	"image/color"
	"time"

	"tinygo.org/x/drivers/ssd1306"
	"tinygo.org/x/tinyfont"
	"tinygo.org/x/tinyfont/proggy"
)

func main() {
	machine.I2C0.Configure(machine.I2CConfig{
		Frequency: machine.TWI_FREQ_400KHZ,
	})

	display := ssd1306.NewI2C(machine.I2C0)
	display.Configure(ssd1306.Config{
		Address: ssd1306.Address_128_32,
		Width:   128,
		Height:  32,
	})

	display.ClearDisplay()

	tinyfont.WriteLine(&display, &proggy.TinySZ8pt7b, 5, 20, []byte("Hello World"), color.RGBA{255, 255, 255, 255})
	//tinyfont.WriteLine(&display, &freeserif.Regular12pt7b, 5, 20, []byte("Hello World"), color.RGBA{255, 255, 255, 255})
	//tinyfont.WriteLine(&display, &freemono.Regular9pt7b, 5, 20, []byte("Hello World"), color.RGBA{255, 255, 255, 255})
	display.Display()
	time.Sleep(20 * time.Second)

	x := int16(0)
	y := int16(0)
	deltaX := int16(1)
	deltaY := int16(1)
	for {
		pixel := display.GetPixel(x, y)
		c := color.RGBA{255, 255, 255, 255}
		if pixel {
			c = color.RGBA{0, 0, 0, 255}
		}
		display.SetPixel(x, y, c)
		display.Display()

		x += deltaX
		y += deltaY

		if x == 0 || x == 127 {
			deltaX = -deltaX
		}

		if y == 0 || y == 31 {
			deltaY = -deltaY
		}
		time.Sleep(1 * time.Millisecond)
	}
}
