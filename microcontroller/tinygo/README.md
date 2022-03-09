## E-Paper Display

### Building and running

Version 1 was built with TinyGo.

1. Install [TinyGo](https://tinygo.org/getting-started/).
1. (NOTE: This step not required anymore?) Install dependencies (only necessary until TinyGo has modules - soon):
    * Install TinyGo drivers: `go get tinygo.org/x/drivers`
    * Install TinyGo fonts: `go get tinygo.org/x/tinyfont`
    * Install TinyGo drawing library: `go get tinygo.org/x/tinydraw`
1. Build and flash the target board: `tinygo flash -target=feather-m0 main.go`
