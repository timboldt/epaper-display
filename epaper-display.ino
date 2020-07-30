#include <Adafruit_GFX.h>
#define ENABLE_GxEPD2_GFX 0
#include "GxEPD2_BW.h"
#include "RTClib.h"

//#include "Fonts/FreeMonoBold9pt7b.h"
//#include "bitmaps/Bitmaps400x300.h"  // 4.2"  b/w

#define MAX_DISPLAY_BUFFER_SIZE 15000ul  // ~15k is a good compromise
#define MAX_HEIGHT(EPD)                                        \
    (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) \
         ? EPD::HEIGHT                                         \
         : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))

// Pin definitions.
#define EPD_CS 12
#define EPD_DC 11
#define EPD_RESET 10
#define EPD_BUSY 6
#define VBATPIN 9
#define POWER_OFF 5

GxEPD2_BW<GxEPD2_420, MAX_HEIGHT(GxEPD2_420)> display(GxEPD2_420(EPD_CS, EPD_DC,
                                                                 EPD_RESET,
                                                                 EPD_BUSY));
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday",   "Monday", "Tuesday", "Wednesday",
                             "Thursday", "Friday", "Saturday"};

void setup() {
    Serial.begin(57600);

    // TEMPORARY: For human benefit.
    // while (!Serial) {
    //     ;  // wait for serial port to connect. Needed for native USB
    // }
    delay(3000);

    // Initialize the real-time clock: DS3231.
    if (!rtc.begin()) {
        Serial.println("RTC not found.");
        Serial.flush();
        abort();
    }
    if (rtc.lostPower()) {
        Serial.println("RTC lost power");
    }

    // Initialize the display: Waveshare 4.2" B/W EPD.
    display.init();

    // Initialize TPL511x Done pin.
    pinMode(POWER_OFF, OUTPUT);
    digitalWrite(POWER_OFF, LOW);

    Serial.println("Init done.");
}

void loop() {
    DateTime now = rtc.now();
    float d23231_temperature = rtc.getTemperature();
    float battery_voltage = BatteryVoltage();

    Serial.printf("Time: %02d:%02d:%02d\n", now.hour(), now.minute(),
                  now.second());
    Serial.printf("DS3231 temperature: %f\n", d23231_temperature);
    Serial.printf("VBat: %f\n", battery_voltage);
    Serial.println();

    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        // drawGuage(&display, 50, 50, "Day", int16(clockTime.Hour()), 7, 22)
        // drawGuage(&display, 50, 100, "Week",
        // int16(clockTime.Weekday())*24+int16(clockTime.Hour()), 0, 7*24)
        // drawGuage(&display, 50, 150, "Month", int16(clockTime.Day()), 1, 31)
        // // drawGuage(&display, 50, 150, "Value: 25%", 25, 0, 100)
        // // drawGuage(&display, 50, 200, "Value: 50%", 50, 0, 100)
        // // drawGuage(&display, 50, 250, "Value: 100%", 100, 0, 100)

        // drawGuage(&display, 350, 50, "Battery", int16(batteryMilliVolts),
        // 3300, 4200) drawGuage(&display, 350, 250, "Inside Temp.",
        // int16(temperatureMilliC/10), 2000, 3000)

        DrawClock(200, 150, now);
    } while (display.nextPage());
    display.hibernate();

    // Signal power off.
    // TODO: Maybe toggle the POWER_OFF to ensure the edge is detected?
    digitalWrite(POWER_OFF, LOW);
    delay(5000);
    digitalWrite(POWER_OFF, HIGH);

    // In case we don't power off, wait at least 3 minutes before redrawing.
    delay(180000);
}

float BatteryVoltage() {
    float raw = analogRead(VBATPIN);
    // 50% voltage divider with a 3.3V reference and 1024 divisions.
    return raw * 2.0 * 3.3 / 1024;
}

void DrawClock(int16_t x, int16_t y, const DateTime &t) {
    const int16_t clock_radius = 105;
    const int16_t clock_inner_radius = 97;
    const int16_t tick_radius = 90;
    const int16_t hour_radius = 55;
    const int16_t minute_radius = 95;
    const int16_t hour_width = 6;
    const int16_t minute_width = 4;
    const int16_t center_point_width = 2;

    // Draw the clock face.
    display.fillCircle(x, y, clock_radius, GxEPD_BLACK);
    display.fillCircle(x, y, clock_inner_radius, GxEPD_WHITE);
    for (int i = 0; i < 12; i++) {
        float dx = sin(float(i) / 12 * 2 * PI);
        float dy = cos(float(i) / 12 * 2 * PI);
        display.drawLine(x + tick_radius * dx, y - tick_radius * dy,
                         x + clock_inner_radius * dx,
                         y - clock_inner_radius * dy, GxEPD_BLACK);
    }

    // Draw the hour hand.
    float hour_angle =
        float(t.twelveHour() * 60 + t.minute()) / 60.0 / 12.0 * 2.0 * PI;
    float hx = sin(hour_angle);
    float hy = cos(hour_angle);
    float hb_x = x - hour_width * hx;
    float hb_y = y + hour_width * hy;
    display.drawTriangle(hb_x - hour_width * hy, hb_y - hour_width * hx,
                         hb_x + hour_width * hy, hb_y + hour_width * hx,
                         x + hour_radius * hx, y - hour_radius * hy,
                         GxEPD_BLACK);

    // Draw the minute hand.
    float minute_angle = float(t.minute()) / 60.0 * 2.0 * PI;
    float mx = sin(minute_angle);
    float my = cos(minute_angle);
    float mb_x = x - minute_width * mx;
    float mb_y = y + minute_width * my;
    display.fillTriangle(mb_x - minute_width * my, mb_y - minute_width * mx,
                         mb_x + minute_width * my, mb_y + minute_width * mx,
                         x + minute_radius * mx, y - minute_radius * my,
                         GxEPD_BLACK);

    // Draw the pin in the center.
    display.fillCircle(x, y, center_point_width, GxEPD_WHITE);
    display.drawCircle(x, y, center_point_width, GxEPD_BLACK);
}

// func drawGuage(display drivers.Displayer, x int16, y int16, label string,
// value int16, minValue int16, maxValue int16) { 	const ( 		guageRadius
// = 20 		tickRadius       = 17 		needleRadius     = 18
// needleWidth      = 3 		centerPointWidth = 2
// 	)
// 	black := color.RGBA{1, 1, 1, 255}
// 	white := color.RGBA{0, 0, 0, 255}

// 	if value < minValue {
// 		value = minValue
// 	}
// 	if value > maxValue {
// 		value = maxValue
// 	}
// 	guageValue := float64(value-minValue)/float64(maxValue-minValue) - 0.5

// 	// Draw the guage face.
// 	tinydraw.Circle(display, x, y, guageRadius, black)
// 	for i := -5; i <= 5; i++ {
// 		dx, dy := math.Sincos(float64(i) / 16 * 2 * math.Pi)
// 		tinydraw.Line(
// 			display,
// 			x+int16(tickRadius*dx), y-int16(tickRadius*dy),
// 			x+int16(guageRadius*dx), y-int16(guageRadius*dy),
// 			black)
// 	}
// 	textWidth, boxWidth := fonts.LineWidth(&fonts.TinySZ8pt7b, []byte(label))
// 	tinydraw.FilledRectangle(
// 		display,
// 		x-int16(boxWidth), y+10,
// 		x+int16(boxWidth), y+24,
// 		white)
// 	fonts.WriteLine(
// 		display,
// 		&fonts.TinySZ8pt7b,
// 		x-int16((textWidth+1)/2), y+guageRadius,
// 		[]byte(label),
// 		black)

// 	// Draw the needle.
// 	angle := guageValue * 10 / 16 * 2 * math.Pi
// 	nx, ny := math.Sincos(angle)
// 	tinydraw.Line(
// 		display,
// 		x+int16(-0.1*needleRadius*nx), y-int16(-0.1*needleRadius*ny),
// 		x+int16(needleRadius*nx), y-int16(needleRadius*ny),
// 		black)

// 	// Draw the pin in the center.
// 	tinydraw.FilledCircle(display, x, y, centerPointWidth, black)
// 	display.SetPixel(x, y, white)
// }
