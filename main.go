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
	// t, _ := time.Parse(time.RFC3339, "2020-07-23T23:32:00-07:00")
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

	machine.InitADC()
	adc := machine.ADC{Pin: machine.D9}
	adc.Configure()
	println("after ADC")

	for {
		// TODO: Handle errors properly.
		rawTemperature, _ := clock.ReadTemperature()
		temperature := float64(rawTemperature) / 1000
		clockTime, _ := clock.ReadTime()
		// Voltage divider is half of 3.3V and total scale is 65536.
		batteryVoltage := float64(adc.Get()) * 2 * 3.3 / 65536
		println(temperature, clockTime.Format(time.RFC3339), batteryVoltage)

		display.ClearDisplay()
		tinyfont.WriteLine(&display, &proggy.TinySZ8pt7b, 0, 6, []byte(
			fmt.Sprintf("%.1f'C  %.2f V",
				temperature,
				batteryVoltage)),
			color.RGBA{255, 255, 255, 255})
		tinyfont.WriteLine(&display, &freesans.Bold12pt7b, 0, 25, []byte(
			fmt.Sprintf("%s",
				clockTime.Format(time.Kitchen))),
			color.RGBA{255, 255, 255, 255})
		display.Display()

		// TODO: Sleep until the start of the next minute.
		time.Sleep(60 * time.Second)
	}
}
