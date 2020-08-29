use arraystring::{typenum::U40, ArrayString};
use core::fmt::*;
use ds323x::NaiveDateTime;
use embedded_graphics::{
    fonts::{Font6x8, Text},
    pixelcolor::BinaryColor,
    prelude::*,
    style::TextStyle,
    //     use embedded_graphics::pixelcolor::BinaryColor;
    // use embedded_graphics::prelude::*;
    // use embedded_graphics::primitives::{Circle, Rectangle, Triangle};
    // use embedded_graphics::style::PrimitiveStyleBuilder;
};
use profont::ProFont24Point;
use epd_waveshare::{epd4in2::*, graphics::Display};

pub struct ClockDisplay {
    disp: Display4in2,
}

impl ClockDisplay {
    pub fn new() -> Self {
        ClockDisplay {
            disp: Display4in2::default(),
        }
    }

    pub fn buffer(&self) -> &[u8] {
        &self.disp.buffer()
    }

    pub fn print_date(&mut self, dt: NaiveDateTime) {
        let mut buf = ArrayString::<U40>::new();
        write!(&mut buf, "{}", dt).expect("Can't write");

        Text::new(&buf, Point::new(2, 28))
            .into_styled(TextStyle::new(ProFont24Point, BinaryColor::On))
            .draw(&mut self.disp)
            .unwrap();
    }
}

// // let mut disp: GraphicsMode<_> = Builder::new()
// //     .size(DisplaySize::Display128x32)
// //     .connect_i2c(i2c_bus.acquire_i2c())
// //     .into();

// // disp.init().unwrap();
// // disp.flush().unwrap();

// let style = PrimitiveStyleBuilder::new()
//     .stroke_color(BinaryColor::On)
//     .stroke_width(1)
//     .build();

// let yoffset = 8;
// let x_max = 127;
// let y_max = 31;

// // screen outline
// if rtc.get_seconds().unwrap() > 30 {
//     Rectangle::new(Point::new(0, 0), Point::new(x_max, y_max))
//         .into_styled(style)
//         .draw(&mut disp)
//         .unwrap();
// }

// // triangle
// // 'Triangle' requires 'embedded_graphics' 0.6 or newer...
// Triangle::new(
//     Point::new(16, 16 + yoffset),
//     Point::new(16 + 16, 16 + yoffset),
//     Point::new(16 + 8, yoffset),
// )
// .into_styled(style)
// .draw(&mut disp)
// .unwrap();

// // square
// Rectangle::new(Point::new(58, yoffset), Point::new(58 + 16, 16 + yoffset))
//     .into_styled(style)
//     .draw(&mut disp)
//     .unwrap();

// // circle
// Circle::new(Point::new(108, yoffset + 8), 8)
//     .into_styled(style)
//     .draw(&mut disp)
//     .unwrap();
