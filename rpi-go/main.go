package main

import (
	"context"
	"fmt"
	"os"

	finnhub "github.com/Finnhub-Stock-API/finnhub-go"
)

func main() {
	finnhubClient := finnhub.NewAPIClient(finnhub.NewConfiguration()).DefaultApi
	auth := context.WithValue(context.Background(), finnhub.ContextAPIKey, finnhub.APIKey{
		Key: os.Getenv("FINHUB_API_KEY"),
	})
	quote, _, err := finnhubClient.Quote(auth, "GOOG")
	if err != nil {
		fmt.Printf("err retrieving quote: %v\n", err)
		os.Exit(1)
	}
	fmt.Printf("%+v\n", quote)
	stockCandles, _, err := finnhubClient.StockCandles(auth, "GOOG", "D", 1590988249, 1591852249, nil)
	if err != nil {
		fmt.Printf("err retrieving candles: %v\n", err)
		os.Exit(1)
	}
	fmt.Printf("%+v\n", stockCandles)
}
