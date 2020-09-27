package main

import (
	"context"
	"fmt"
	"log"
	"os"

	finnhub "github.com/Finnhub-Stock-API/finnhub-go"
	owm "github.com/briandowns/openweathermap"
	"periph.io/x/periph/conn/i2c/i2creg"
	"periph.io/x/periph/conn/physic"
	"periph.io/x/periph/devices/bmxx80"
	"periph.io/x/periph/host"
)

func main() {
	//
	// BME280
	//

	// Load all the drivers:
	fmt.Println("host.Init")
	if _, err := host.Init(); err != nil {
		log.Fatal(err)
	}

	// Open a handle to the first available I²C bus:
	fmt.Println("i2creg.open")
	bus, err := i2creg.Open("")
	if err != nil {
		log.Fatal(err)
	}
	defer bus.Close()

	// Open a handle to a bme280/bmp280 connected on the I²C bus using default
	// settings:
	fmt.Println("bmxx80.NewI2C")
	dev, err := bmxx80.NewI2C(bus, 0x76, &bmxx80.DefaultOpts)
	if err != nil {
		log.Fatal(err)
	}
	defer dev.Halt()

	// Read temperature from the sensor:
	var env physic.Env
	if err = dev.Sense(&env); err != nil {
		log.Fatal(err)
	}
	fmt.Printf("%8s %10s %9s\n", env.Temperature, env.Pressure, env.Humidity)

	//
	// FinnHub
	//
	finnhubClient := finnhub.NewAPIClient(finnhub.NewConfiguration()).DefaultApi
	auth := context.WithValue(context.Background(), finnhub.ContextAPIKey, finnhub.APIKey{
		Key: os.Getenv("FINHUB_API_KEY"),
	})
	quote, _, err := finnhubClient.Quote(auth, "GOOG")
	if err != nil {
		log.Fatalln(err)
	}
	fmt.Printf("%+v\n", quote)
	stockCandles, _, err := finnhubClient.StockCandles(auth, "GOOG", "D", 1590988249, 1591852249, nil)
	if err != nil {
		log.Fatalln(err)
	}
	fmt.Printf("%+v\n", stockCandles)

	//
	// OpenWeatherMap
	//
	w, err := owm.NewCurrent("C", "EN", os.Getenv("OPEN_WEATHER_KEY"))
	if err != nil {
		log.Fatalln(err)
	}
	w.CurrentByID(5383777)
	fmt.Println(w)
}
