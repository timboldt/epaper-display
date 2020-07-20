## E-Paper Display

### Disclaimer

This is not an officially supported Google product.

### Introduction

This project is an experiment to make a low-power desktop clock, using a Waveshare EPD display.

### Building and running

1. Install [TinyGo](https://tinygo.org/getting-started/).
1. Install dependencies (only necessary until TinyGo has modules - soon):
    * Install TinyGo drivers: `go get tinygo.org/x/drivers`
    * Install TinyGo fonts: `go get tinygo.org/x/tinyfont`
    * Install TinyGo drawing library: `go get tinygo.org/x/tinydraw`
1. Build and flash the target board: `tinygo flash -target=feather-m0 main.go`
