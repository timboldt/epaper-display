#include <Adafruit_GFX.h>
#include <Fonts/Picopixel.h>
#include <GxEPD2_BW.h>
#include <RTClib.h>
#include <SPI.h>
#include <WiFiNINA.h>

#define MAX_DISPLAY_BUFFER_SIZE 15000ul  // ~15k is a good compromise
#define MAX_HEIGHT(EPD)                                        \
    (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) \
         ? EPD::HEIGHT                                         \
         : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))

//==================
// Pin definitions.
//==================

// Waveshare EPD.
#define EPD_BUSY A2
#define EPD_RESET A3
#define EPD_DC A4
#define EPD_CS A5

// TPL5111.
#define POWER_OFF 5

// Voltage divider for LiPo.
#define VBATPIN 9

// Adafruit ESP32 Airlift.
#define SPIWIFI SPI      // The SPI port
#define SPIWIFI_SS 13    // Chip select pin
#define ESP32_RESETN 12  // Reset pin
#define SPIWIFI_ACK 11   // a.k.a BUSY or READY pin
#define ESP32_GPIO0 -1

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

    // WiFi setup.
    WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
    while (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        delay(1000);
    }
    String fv = WiFi.firmwareVersion();
    Serial.println(fv);
    if (fv < "1.0.0") {
        Serial.println("Please upgrade the firmware");
        while (1) delay(10);
    }
    Serial.println("Firmware OK");

    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC: ");
    printMacAddress(mac);

    Serial.println("Scanning available networks...");
    listNetworks();

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
        DrawGauge(50, 50, "Work Day", now.hour(), 8, 17);
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
