package main

import (
	"fmt"
	"machine"

	"image/color"
	"time"

	"tinygo.org/x/drivers/ds3231"
	"tinygo.org/x/tinyfont"
	"tinygo.org/x/tinyfont/freesans"
	"tinygo.org/x/tinyfont/proggy"

	// TODO: Switch to Go modules, when available in TinyGo.
	"./epd4in2"
)

func main() {
	// TEMPORARY: For human benefit.
	time.Sleep(3 * time.Second)

	//
	// === Initialize hardware ===
	//

	// Initialize common hardware.
	machine.InitADC()
	machine.I2C0.Configure(machine.I2CConfig{
		Frequency: machine.TWI_FREQ_100KHZ,
	})
	machine.SPI0.Configure(machine.SPIConfig{
		Frequency: 8000000,
		Mode:      0,
	})

	// Initialize Clock.
	clock := ds3231.New(machine.I2C0)
	clock.Configure()

	// Initialize Battery Voltage ADC.
	vbat := machine.ADC{Pin: machine.D9}
	vbat.Configure()

	// Initialize E-Paper display.
	display := epd4in2.New(machine.SPI0, machine.D12, machine.D11, machine.D10, machine.D6)
	display.Configure(epd4in2.Config{})
	black := color.RGBA{1, 1, 1, 255}
	//white := color.RGBA{0, 0, 0, 255}
	display.ClearBuffer()
	println("epd: ClearDisplay")
	display.ClearDisplay()
	println("epd: WaitUntilIdle")
	display.WaitUntilIdle()
	println("epd: Pause for 2 seconds")
	time.Sleep(2 * time.Second)

	for {
		//
		// === Read the sensors ===
		//
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
		batteryMilliVolts := int32(vbat.Get()) * 2 * 3300 / 65536

		//
		// === Update the display ===
		//
		statusInfo := fmt.Sprintf("%d.%02d C  %d.%02d V",
			temperatureMilliC/1000, temperatureMilliC%1000/10,
			batteryMilliVolts/1000, batteryMilliVolts%1000/10)
		println(clockTime.Format(time.Kitchen), statusInfo)

		display.ClearBuffer()
		tinyfont.WriteLine(&display, &proggy.TinySZ8pt7b, 0, 6, []byte(statusInfo), black)
		tinyfont.WriteLine(&display, &freesans.Bold12pt7b, 0, 25, []byte(
			fmt.Sprintf("%s",
				clockTime.Format(time.Kitchen))),
			black)
		println("epd: Display")
		display.Display()
		println("epd: WaitUntilIdle")
		display.WaitUntilIdle()
		println("epd: DeepSleep")
		display.DeepSleep()

		// TODO: Sleep until the start of the next minute.
		time.Sleep(180 * time.Second)
	}
}
