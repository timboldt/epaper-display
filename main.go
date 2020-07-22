package main

import (
	"fmt"
	"machine"

	"image/color"
	"time"

	"tinygo.org/x/drivers/ds3231"
	"tinygo.org/x/drivers/ssd1306"
	"tinygo.org/x/tinyfont"
	"tinygo.org/x/tinyfont/freesans"
	"tinygo.org/x/tinyfont/proggy"
)

func main() {
	time.Sleep(3 * time.Second)
	println("before")

	machine.I2C0.Configure(machine.I2CConfig{
		Frequency: machine.TWI_FREQ_400KHZ,
	})

	clock := ds3231.New(machine.I2C0)
	clock.Configure()
	// t, _ := time.Parse(time.RFC3339, "2020-07-21T19:44:00-07:00")
	// clock.SetTime(t)
	println("after clock")

	display := ssd1306.NewI2C(machine.I2C0)
	display.Configure(ssd1306.Config{
		Address: ssd1306.Address_128_32,
		Width:   128,
		Height:  32,
	})
	display.ClearDisplay()
	tinyfont.WriteLine(&display, &proggy.TinySZ8pt7b, 5, 20, []byte("Hello World"), color.RGBA{255, 255, 255, 255})
	display.Display()
	println("after display")

	for {
		temp, _ := clock.ReadTemperature()
		tim, _ := clock.ReadTime()
		println(temp, tim.Format(time.RFC3339))

		display.ClearDisplay()
		tinyfont.WriteLine(&display, &proggy.TinySZ8pt7b, 0, 6, []byte(fmt.Sprintf("Temp: %.2f C", float64(temp)/1000)), color.RGBA{255, 255, 255, 255})
		tinyfont.WriteLine(&display, &freesans.Bold12pt7b, 0, 25, []byte(fmt.Sprintf("%s", tim.Format(time.Kitchen))), color.RGBA{255, 255, 255, 255})
		display.Display()

		time.Sleep(60 * time.Second)
	}
}
