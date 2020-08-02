#include "global.h"

void setup() {
    Serial.begin(57600);

    // Initialize the display: Waveshare 4.2" B/W EPD.
    display.init();

    InitDS3231();
    InitBME280();

    // Initialize TPL511x Done pin.
    pinMode(POWER_OFF, OUTPUT);
    digitalWrite(POWER_OFF, LOW);

    if (ConnectToNetwork()) {
        SetTimeFromWeb();
        DisconnectFromNetwork();
    }

    Serial.println("Init done.");
}

void loop() {
    float outsideTemperature, outsideHumidity;
    if (ConnectToNetwork()) {
        GetWeatherFromWeb(&outsideTemperature, &outsideHumidity);
        DisconnectFromNetwork();
    }

    float temperature, humidity, pressure;
    ReadBME280(&temperature, &humidity, &pressure);

    DateTime now = rtc.now();
    float d23231_temperature = rtc.getTemperature();

    float battery_voltage = BatteryVoltage();

    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        DrawGauge(50, 50, "Work Day", now.hour(), 8, 17);
        DrawGauge(50, 100, "Work Week", now.dayOfTheWeek(), 1, 5);
        DrawGauge(50, 150, "Month", now.day(), 1, 31);
        DrawGauge(50, 200, "Battery", battery_voltage, 3.3, 4.2);
        DrawGauge(50, 250, "Clock Temp.", d23231_temperature, 20, 30);

        DrawGauge(350, 50, "Inside Temp.", temperature, 20, 30);
        DrawGauge(350, 100, "Inside Humid.", humidity, 0, 100);
        DrawGauge(350, 150, "Pressure", pressure, 99, 101);
        DrawGauge(350, 200, "Outside Temp.", outsideTemperature, 20, 30);
        DrawGauge(350, 250, "Outside Humid.", outsideHumidity, 0, 100);

        DrawDate(now);
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
