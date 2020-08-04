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
    float outsideTemperature = 0.0;
    float outsideHumidity = 0.0;
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
        DrawGauge(260, 60, "Work Day", now.hour(), 8, 17);
        DrawGauge(310, 60, "Work Week", now.dayOfTheWeek(), 1, 5);
        DrawGauge(360, 60, "Month", now.day(), 1, 31);

        DrawGauge(260, 110, "Battery", battery_voltage, 3.3, 4.2);
        DrawGauge(360, 110, "Clock Tmp", d23231_temperature, 20, 30);

        DrawChoice(260, 160, temperature, (now.hour() < 22) ? 23 : 18,
                   outsideTemperature);
        DrawGauge(310, 160, "Inside Tmp", temperature, 20, 30);
        DrawGauge(360, 160, "Outside Tmp", outsideTemperature, 20, 30);

        //DrawChoice(260, 210, humidity, 40, outsideHumidity);
        DrawGauge(260, 210, "Pressure", pressure, 99, 101);
        DrawGauge(310, 210, "Inside RH%", humidity, 0, 100);
        DrawGauge(360, 210, "Outside RH%", outsideHumidity, 0, 100);

        DrawDate(now);
        DrawClock(120, 120, now);
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
