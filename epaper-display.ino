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
        DrawGauge(50, 50, "Work Day", now.hour(), 8, 5);
        DrawGauge(50, 100, "Work Week", now.dayOfTheWeek(), 1, 5);
        DrawGauge(50, 150, "Month", now.day(), 1, 31);

        DrawGauge(350, 50, "Battery", battery_voltage, 3.3, 4.2);
        DrawGauge(350, 250, "Inside Temp.", d23231_temperature, 20, 30);

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

void DrawGauge(int16_t x, int16_t y, const char *label, float value,
               float min_value, float max_value) {
    const int16_t gauge_radius = 20;
    const int16_t tick_radius = 17;
    const int16_t needle_radius = 18;
    const int16_t needle_width = 3;
    const int16_t pin_width = 2;

    if (value < min_value) {
        value = min_value;
    }
    if (value > max_value) {
        value = max_value;
    }
    float gauge_value = (value - min_value) / (max_value - min_value) - 0.5;

    // Draw the guage face.
    display.drawCircle(x, y, gauge_radius, GxEPD_BLACK);
    for (int i = -5; i <= 5; i++) {
        float dx = sin(i * 2 * PI / 16);
        float dy = cos(i * 2 * PI / 16);
        display.drawLine(x + tick_radius * dx, y - tick_radius * dy,
                         x + gauge_radius * dx, y - gauge_radius * dy,
                         GxEPD_BLACK);
    }
    display.fillRect(x - gauge_radius, y + 10, x + gauge_radius, y + 24,
                     GxEPD_WHITE);
    // textWidth, boxWidth := fonts.LineWidth(&fonts.TinySZ8pt7b, []byte(label))
    // fonts.WriteLine(
    // 	display,
    // 	&fonts.TinySZ8pt7b,
    // 	x-int16((textWidth+1)/2), y+guageRadius,
    // 	[]byte(label),
    // 	black)

    // Draw the needle.
    float angle = gauge_value * 10 / 16 * 2 * PI;
    float nx = sin(angle);
    float ny = cos(angle);
    display.drawLine(x - 0.1 * needle_radius * nx, y + 0.1 * needle_radius * ny,
                     x + needle_radius * nx, y - needle_radius * ny,
                     GxEPD_BLACK);

    // Draw the pin in the center.
    display.fillCircle(x, y, pin_width, GxEPD_BLACK);
    display.drawPixel(x, y, GxEPD_WHITE);
}
