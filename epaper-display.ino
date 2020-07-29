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
    while (!Serial) {
        ;  // wait for serial port to connect. Needed for native USB
    }
    // delay(3000);

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

    Serial.printf("Time: %02d:%02d:%02d\n", now.hour(), now.minute(), now.second());
    Serial.printf("DS3231 temperature: %f\n", d23231_temperature);
    Serial.printf("VBat: %f\n", battery_voltage);
    Serial.println();

    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.fillRect(0, 0, 100, 100, GxEPD_BLACK);
    } while (display.nextPage());
    display.hibernate();
    digitalWrite(POWER_OFF, HIGH);
    // TODO: Maybe toggle the POWER_OFF to ensure the edge is detected?
    delay(180000);
}

float BatteryVoltage() {
    float raw = analogRead(VBATPIN);
    // 50% voltage divider with a 3.3V reference and 1024 divisions.
    return raw * 2.0 * 3.3 / 1024;
}
