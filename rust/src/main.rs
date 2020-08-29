//  Copyright 2020 Google LLC
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#![no_std]
#![no_main]

extern crate arraystring;
extern crate ds323x;
extern crate embedded_graphics;
extern crate embedded_hal;
extern crate epd_waveshare;
extern crate feather_m0 as hal;
extern crate panic_halt;
extern crate profont;
extern crate shared_bus;

mod ui;

use embedded_hal::{blocking::delay::*, digital::v2::*};
use hal::clock::GenericClockController;
use hal::delay::Delay;
use hal::entry;
use hal::pac::{CorePeripherals, Peripherals, SCB};
//use hal::prelude::*;
use hal::time::{KiloHertz, MegaHertz};

use ds323x::{Ds323x, Rtcc};
use epd_waveshare::{epd4in2::*, prelude::*};

#[entry]
fn main() -> ! {
    let mut peripherals = Peripherals::take().unwrap();
    let core = CorePeripherals::take().unwrap();
    let mut clocks = GenericClockController::with_internal_32kosc(
        peripherals.GCLK,
        &mut peripherals.PM,
        &mut peripherals.SYSCTRL,
        &mut peripherals.NVMCTRL,
    );
    let mut delay = Delay::new(core.SYST, &mut clocks);
    let mut pins = hal::Pins::new(peripherals.PORT);

    // Before doing anything else, pull the line low that is connected to the TPL5111 done pin.
    let mut power_switch = pins.d5.into_push_pull_output(&mut pins.port);
    power_switch.set_low().unwrap();

    let mut red_led = pins.d13.into_open_drain_output(&mut pins.port);

    // Set up the common, shared SPI bus.
    let spi_bus = shared_bus::BusManagerSimple::new(hal::spi_master(
        &mut clocks,
        MegaHertz(10),
        peripherals.SERCOM4,
        &mut peripherals.PM,
        pins.sck,
        pins.mosi,
        pins.miso,
        &mut pins.port,
    ));

    // Set up the common, shared I2C bus.
    let i2c_bus = shared_bus::BusManagerSimple::new(hal::i2c_master(
        &mut clocks,
        KiloHertz(400),
        peripherals.SERCOM3,
        &mut peripherals.PM,
        pins.sda,
        pins.scl,
        &mut pins.port,
    ));

    /*
    // Voltage divider for LiPo.
    #define VBATPIN 9

    // Adafruit ESP32 Airlift.
    #define SPIWIFI SPI      // The SPI port
    #define SPIWIFI_SS 13    // Chip select pin
    #define ESP32_RESETN 12  // Reset pin
    #define SPIWIFI_ACK 11   // a.k.a BUSY or READY pin
    #define ESP32_GPIO0 -1
        */

    // Give the peripherals time to start up before talking to them.
    delay.delay_ms(100u16);

    let mut rtc = Ds323x::new_ds3231(i2c_bus.acquire_i2c());
    let _clk_temp = rtc.get_temperature().unwrap();
    let clk_time = rtc.get_datetime().unwrap();

    let mut clock_display = ui::ClockDisplay::new();
    clock_display.print_date(clk_time);

    // Display updated frame
    let epd_busy = pins.a2.into_floating_input(&mut pins.port);
    let epd_reset = pins.a3.into_open_drain_output(&mut pins.port);
    let epd_dc = pins.a4.into_open_drain_output(&mut pins.port);
    let epd_cs = pins.a5.into_open_drain_output(&mut pins.port);
    let mut epd_spi = spi_bus.acquire_spi();
    let mut epd = EPD4in2::new(
        &mut epd_spi,
        epd_cs,
        epd_busy,
        epd_dc,
        epd_reset,
        &mut delay,
    )
    .unwrap();
    epd.update_frame(&mut epd_spi, &clock_display.buffer())
        .unwrap();
    epd.display_frame(&mut epd_spi).unwrap();
    epd.sleep(&mut epd_spi).unwrap();

    red_led.set_high().unwrap();

    // Tell the TPL5111 to turn off the power.
    power_switch.set_low().unwrap();
    delay.delay_ms(1_u16);
    power_switch.set_high().unwrap();

    // If power-off fails, sleep for a long time and then reset.
    delay.delay_ms(60_000_u16);
    SCB::sys_reset();
}
