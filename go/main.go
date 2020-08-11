package main

import (
	"fmt"
	"machine"
	"math"

	"image/color"
	"time"

	"tinygo.org/x/drivers"
	"tinygo.org/x/drivers/ds3231"
	"tinygo.org/x/tinydraw"
	"tinygo.org/x/tinyfont"
	"tinygo.org/x/tinyfont/proggy"

	// TODO: Switch to Go modules, when available in TinyGo.
	"github.com/timboldt/epaper-display/go/epd4in2"
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

	// Initialize TPL511x done pin.
	powerOff := machine.D5
	powerOff.Configure(machine.PinConfig{Mode: machine.PinOutput})
	powerOff.Low()

	// Initialize E-Paper display.
	display := epd4in2.New(machine.SPI0, machine.A5, machine.A4, machine.A3, machine.A2)
	display.Configure(epd4in2.Config{})
	display.ClearBuffer()
	println("epd: ClearDisplay")
	display.ClearDisplay()
	println("epd: WaitUntilIdle")
	display.WaitUntilIdle()
	println("epd: Pause for 10 seconds")
	time.Sleep(10 * time.Second)

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

		drawGuage(&display, 50, 50, "Day", int16(clockTime.Hour()), 7, 22)
		drawGuage(&display, 50, 100, "Week", int16(clockTime.Weekday())*24+int16(clockTime.Hour()), 0, 7*24)
		drawGuage(&display, 50, 150, "Month", int16(clockTime.Day()), 1, 31)
		// drawGuage(&display, 50, 150, "Value: 25%", 25, 0, 100)
		// drawGuage(&display, 50, 200, "Value: 50%", 50, 0, 100)
		// drawGuage(&display, 50, 250, "Value: 100%", 100, 0, 100)

		drawGuage(&display, 350, 50, "Battery", int16(batteryMilliVolts), 3300, 4200)
		drawGuage(&display, 350, 250, "Inside Temp.", int16(temperatureMilliC/10), 2000, 3000)

		drawClock(&display, 200, 150, clockTime)
		println("epd: Display")
		display.Display()
		println("epd: WaitUntilIdle")
		display.WaitUntilIdle()
		println("epd: DeepSleep")
		display.DeepSleep()

		time.Sleep(5 * time.Second)

		// Signal power off.
		powerOff.Low()
		time.Sleep(1 * time.Millisecond)
		powerOff.High()

		// Sleep until the start of the next appropriate minute.
		seconds := int64(180 - clockTime.Second())
		time.Sleep(time.Duration(seconds) * time.Second)
	}
}

func drawClock(display drivers.Displayer, x int16, y int16, t time.Time) {
	const (
		clockRadius      = 105
		clockInnerRadius = 97
		tickRadius       = 90
		hourRadius       = 55
		minuteRadius     = 95
		hourWidth        = 6
		minuteWidth      = 5
		centerPointWidth = 2
	)
	black := color.RGBA{1, 1, 1, 255}
	white := color.RGBA{0, 0, 0, 255}

	// Draw the clock face.
	tinydraw.FilledCircle(display, x, y, clockRadius, black)
	tinydraw.FilledCircle(display, x, y, clockInnerRadius, white)
	for i := 0; i < 12; i++ {
		dx, dy := math.Sincos(float64(i) / 12 * 2 * math.Pi)
		tinydraw.Line(
			display,
			x+int16(tickRadius*dx), y-int16(tickRadius*dy),
			x+int16(clockInnerRadius*dx), y-int16(clockInnerRadius*dy),
			black)
	}

	// Draw the clock hands.
	minuteAngle := float64(t.Minute()) / 60 * 2 * math.Pi
	mx, my := math.Sincos(minuteAngle)
	hourAngle := float64(t.Hour()*60+t.Minute()) / 60 / 12 * 2 * math.Pi
	hx, hy := math.Sincos(hourAngle)
	tinydraw.Triangle(
		display,
		x-int16(hourWidth*hy), y-int16(hourWidth*hx),
		x+int16(hourWidth*hy), y+int16(hourWidth*hx),
		x+int16(hourRadius*hx), y-int16(hourRadius*hy),
		black)
	tinydraw.Line(display, x, y, x+int16(hourRadius*hx), y-int16(hourRadius*hy), black)
	// tinydraw.Line(display, x, y, x+int16(minuteRadius*mx), y-int16(minuteRadius*my), black)
	tinydraw.FilledTriangle(
		display,
		x-int16(minuteWidth*my), y-int16(minuteWidth*mx),
		x+int16(minuteWidth*my), y+int16(minuteWidth*mx),
		x+int16(minuteRadius*mx), y-int16(minuteRadius*my),
		black)

	// Draw the pin in the center.
	tinydraw.FilledCircle(display, x, y, centerPointWidth, white)
	tinydraw.Circle(display, x, y, centerPointWidth, black)
}

func drawGuage(display drivers.Displayer, x int16, y int16, label string, value int16, minValue int16, maxValue int16) {
	const (
		guageRadius      = 20
		tickRadius       = 17
		needleRadius     = 18
		needleWidth      = 3
		centerPointWidth = 2
	)
	black := color.RGBA{1, 1, 1, 255}
	white := color.RGBA{0, 0, 0, 255}

	if value < minValue {
		value = minValue
	}
	if value > maxValue {
		value = maxValue
	}
	guageValue := float64(value-minValue)/float64(maxValue-minValue) - 0.5

	// Draw the guage face.
	tinydraw.Circle(display, x, y, guageRadius, black)
	for i := -5; i <= 5; i++ {
		dx, dy := math.Sincos(float64(i) / 16 * 2 * math.Pi)
		tinydraw.Line(
			display,
			x+int16(tickRadius*dx), y-int16(tickRadius*dy),
			x+int16(guageRadius*dx), y-int16(guageRadius*dy),
			black)
	}
	textWidth, boxWidth := tinyfont.LineWidth(&proggy.TinySZ8pt7b, label)
	tinydraw.FilledRectangle(
		display,
		x-int16(boxWidth), y+10,
		x+int16(boxWidth), y+24,
		white)
	tinyfont.WriteLine(
		display,
		&proggy.TinySZ8pt7b,
		x-int16((textWidth+1)/2), y+guageRadius,
		label,
		black)

	// Draw the needle.
	angle := guageValue * 10 / 16 * 2 * math.Pi
	nx, ny := math.Sincos(angle)
	tinydraw.Line(
		display,
		x+int16(-0.1*needleRadius*nx), y-int16(-0.1*needleRadius*ny),
		x+int16(needleRadius*nx), y-int16(needleRadius*ny),
		black)

	// Draw the pin in the center.
	tinydraw.FilledCircle(display, x, y, centerPointWidth, black)
	display.SetPixel(x, y, white)
}
