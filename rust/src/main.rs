#![no_std]
#![no_main]

extern crate ds323x;
extern crate embedded_graphics;
extern crate embedded_hal;
extern crate epd_waveshare;
extern crate feather_m0 as hal;
extern crate panic_halt;
extern crate shared_bus;

use embedded_hal::{
    blocking::{delay::*, spi::Write},
    digital::v2::*,
};
use hal::clock::GenericClockController;
use hal::delay::Delay;
use hal::entry;
use hal::pac::{CorePeripherals, Peripherals, SCB};
use hal::prelude::*;
use hal::time::{MegaHertz, KiloHertz};

use ds323x::{Ds323x, NaiveDateTime, Rtcc};
use embedded_graphics::pixelcolor::BinaryColor;
use embedded_graphics::prelude::*;
use embedded_graphics::primitives::{Circle, Rectangle, Triangle};
use embedded_graphics::style::PrimitiveStyleBuilder;
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

    let mut spi = hal::spi_master(
        &mut clocks,
        MegaHertz(10),
        peripherals.SERCOM4,
        &mut peripherals.PM,
        pins.sck,
        pins.mosi,
        pins.miso,
        &mut pins.port,
    );
    let epd_busy = pins.a2.into_floating_input(&mut pins.port);
    let epd_reset = pins.a3.into_open_drain_output(&mut pins.port);
    let epd_dc = pins.a4.into_open_drain_output(&mut pins.port);
    let epd_cs = pins.a5.into_open_drain_output(&mut pins.port);
    let mut epd = EPD4in2::new(&mut spi, epd_cs, epd_busy, epd_dc, epd_reset, &mut delay).unwrap();

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

    let i2c = hal::i2c_master(
        &mut clocks,
        KiloHertz(400),
        peripherals.SERCOM3,
        &mut peripherals.PM,
        pins.sda,
        pins.scl,
        &mut pins.port,
    );

    let i2c_bus = shared_bus::BusManagerSimple::new(i2c);

    // Give the peripherals time to start up before talking to them.
    delay.delay_ms(100u16);

    let mut rtc = Ds323x::new_ds3231(i2c_bus.acquire_i2c());
    let _clk_temp = rtc.get_temperature().unwrap();
    let _clk_time = rtc.get_datetime().unwrap();
    //println!("Time: {}", time);

    let mut disp = Display4in2::default();

    // let mut disp: GraphicsMode<_> = Builder::new()
    //     .size(DisplaySize::Display128x32)
    //     .connect_i2c(i2c_bus.acquire_i2c())
    //     .into();

    // disp.init().unwrap();
    // disp.flush().unwrap();

    let style = PrimitiveStyleBuilder::new()
        .stroke_color(BinaryColor::On)
        .stroke_width(1)
        .build();

    let yoffset = 8;
    let x_max = 127;
    let y_max = 31;

    // screen outline
    if rtc.get_seconds().unwrap() > 30 {
    Rectangle::new(Point::new(0, 0), Point::new(x_max, y_max))
        .into_styled(style)
        .draw(&mut disp)
        .unwrap();
    }

    // triangle
    // 'Triangle' requires 'embedded_graphics' 0.6 or newer...
    Triangle::new(
        Point::new(16, 16 + yoffset),
        Point::new(16 + 16, 16 + yoffset),
        Point::new(16 + 8, yoffset),
    )
    .into_styled(style)
    .draw(&mut disp)
    .unwrap();

    // square
    Rectangle::new(Point::new(58, yoffset), Point::new(58 + 16, 16 + yoffset))
        .into_styled(style)
        .draw(&mut disp)
        .unwrap();

    // circle
    Circle::new(Point::new(108, yoffset + 8), 8)
        .into_styled(style)
        .draw(&mut disp)
        .unwrap();

    // Display updated frame
    epd.update_frame(&mut spi, &disp.buffer()).unwrap();
    epd.display_frame(&mut spi).unwrap();
    epd.sleep(&mut spi).unwrap();

    red_led.set_high().unwrap();

    // Tell the TPL5111 to turn off the power.
    power_switch.set_low().unwrap();
    delay.delay_ms(1_u16);
    power_switch.set_high().unwrap();

    // If power-off fails, sleep for a long time and then reset.
    delay.delay_ms(60_000_u16);
    SCB::sys_reset();
}
