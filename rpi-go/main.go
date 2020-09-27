package main

import (
	"context"
	"fmt"
	"log"
	"os"

	finnhub "github.com/Finnhub-Stock-API/finnhub-go"
	owm "github.com/briandowns/openweathermap"
)

func main() {
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
