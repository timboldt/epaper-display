#![no_std]
#![no_main]

extern crate ds323x;
extern crate embedded_graphics;
extern crate feather_m0 as hal;
extern crate panic_halt;
extern crate shared_bus;
extern crate ssd1306;

use hal::clock::GenericClockController;
use hal::delay::Delay;
use hal::entry;
use hal::pac::{CorePeripherals, Peripherals};
use hal::prelude::*;
use hal::time::KiloHertz;

use ds323x::{Ds323x, NaiveDateTime, Rtcc};
use embedded_graphics::pixelcolor::BinaryColor;
use embedded_graphics::prelude::*;
use embedded_graphics::primitives::{Circle, Rectangle, Triangle};
use embedded_graphics::style::PrimitiveStyleBuilder;
use ssd1306::prelude::*;
use ssd1306::Builder;

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
    let mut pins = hal::Pins::new(peripherals.PORT);
    let mut red_led = pins.d13.into_open_drain_output(&mut pins.port);
    let mut delay = Delay::new(core.SYST, &mut clocks);

    delay.delay_ms(800u16);

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

    let mut rtc = Ds323x::new_ds3231(i2c_bus.acquire_i2c());
    let _clk_temp = rtc.get_temperature().unwrap();
    let _clk_time = rtc.get_datetime().unwrap();
    //println!("Time: {}", time);

    let mut disp: GraphicsMode<_> = Builder::new()
        .size(DisplaySize::Display128x32)
        .connect_i2c(i2c_bus.acquire_i2c())
        .into();

    disp.init().unwrap();
    disp.flush().unwrap();

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

    disp.flush().unwrap();

    // blink the onboard blinkenlight (digital pin 13)
    loop {
        delay.delay_ms(800u16);
        red_led.set_high().unwrap();
        delay.delay_ms(800u16);
        red_led.set_low().unwrap();
    }
}
