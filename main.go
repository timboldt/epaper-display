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

	machine.I2C0.Configure(machine.I2CConfig{
		Frequency: machine.TWI_FREQ_100KHZ,
	})

	clock := ds3231.New(machine.I2C0)
	clock.Configure()

	display := ssd1306.NewI2C(machine.I2C0)
	display.Configure(ssd1306.Config{
		Address: ssd1306.Address_128_32,
		Width:   128,
		Height:  32,
	})
	display.ClearDisplay()
	tinyfont.WriteLine(&display, &proggy.TinySZ8pt7b, 5, 20, []byte("Hello World"), color.RGBA{255, 255, 255, 255})
	display.Display()

	machine.InitADC()
	adc := machine.ADC{Pin: machine.D9}
	adc.Configure()

	for {
		temperatureMilliC, err := clock.ReadTemperature()
		if err != nil {
			println("Error reading temperature", err)
		}
		clockTime, err := clock.ReadTime()
		if err != nil {
			println("Error reading time", err)
			clockTime = time.Time{}
		}
		// Voltage divider is half of 3.3V and total scale is 65536.
		batteryMilliVolts := int32(adc.Get()) * 2 * 3300 / 65536

		statusInfo := fmt.Sprintf("%d.%02d C  %d.%02d V",
			temperatureMilliC/1000, temperatureMilliC%1000/10,
			batteryMilliVolts/1000, batteryMilliVolts%1000/10)

		display.ClearDisplay()
		tinyfont.WriteLine(&display, &proggy.TinySZ8pt7b, 0, 6, []byte(statusInfo), color.RGBA{255, 255, 255, 255})
		tinyfont.WriteLine(&display, &freesans.Bold12pt7b, 0, 25, []byte(
			fmt.Sprintf("%s",
				clockTime.Format(time.Kitchen))),
			color.RGBA{255, 255, 255, 255})
		display.Display()

		// TODO: Sleep until the start of the next minute.
		time.Sleep(1 * time.Second)
	}
}
