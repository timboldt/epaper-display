#include "global.h"

void setup() {
    Serial.begin(57600);

    // Uncomment the following for testing.
    // while (!Serial) {
    //     delay(100);
    // }

    // Initialize the display: Waveshare 4.2" B/W EPD.
    display.init();

    InitDS3231();
    InitBME280();

    // Initialize TPL511x Done pin.
    pinMode(POWER_OFF, OUTPUT);
    digitalWrite(POWER_OFF, LOW);

    if (fram.begin()) {
        Serial.println("FRAM found.");
    }
    MountFilesystem();

    if (rtc.lostPower()) {
        if (ConnectToNetwork()) {
            SetTimeFromWeb();
            DisconnectFromNetwork();
        }
    }

    Serial.println("Init done.");
}

void loop() {
    NvmCacheInit();

    float outsideTemperature = NvmCacheGet(CACHEKEY_OUTSIDE_TEMPERATURE);
    float outsideHumidity = NvmCacheGet(CACHEKEY_OUTSIDE_HUMIDITY);
    float ozone = NvmCacheGet(CACHEKEY_AQI_OZONE);
    float pm25 = NvmCacheGet(CACHEKEY_AQI_PM25);
    float btc = NvmCacheGet(CACHEKEY_MARKETS_BTC);
    float goog = NvmCacheGet(CACHEKEY_MARKETS_GOOG);
    float googPctChange = NvmCacheGet(CACHEKEY_MARKETS_GOOG_PCT);
    float sp500 = NvmCacheGet(CACHEKEY_MARKETS_SP500);
    float sp500PctChange = NvmCacheGet(CACHEKEY_MARKETS_SP500_PCT);
    float lastNetConnect = NvmCacheGet(CACHEKEY_LAST_NET);

    const float NET_REFRESH_SECONDS = 900.0f;
    if (lastNetConnect + NET_REFRESH_SECONDS < float(rtc.now().unixtime())) {
        if (ConnectToNetwork()) {
            SetTimeFromWeb();
            GetWeatherFromWeb(&outsideTemperature, &outsideHumidity);
            GetAQIFromWeb(&ozone, &pm25);
            GetBTCFromWeb(&btc);
            GetGOOGFromWeb(&goog, &googPctChange);
            GetSP500FromWeb(&sp500, &sp500PctChange);

            DisconnectFromNetwork();

            NvmCacheSet(CACHEKEY_OUTSIDE_TEMPERATURE, outsideTemperature);
            NvmCacheSet(CACHEKEY_OUTSIDE_HUMIDITY, outsideHumidity);
            NvmCacheSet(CACHEKEY_AQI_OZONE, ozone);
            NvmCacheSet(CACHEKEY_AQI_PM25, pm25);
            NvmCacheSet(CACHEKEY_MARKETS_BTC, btc);
            NvmCacheSet(CACHEKEY_MARKETS_GOOG, goog);
            NvmCacheSet(CACHEKEY_MARKETS_GOOG_PCT, googPctChange);
            NvmCacheSet(CACHEKEY_MARKETS_SP500, sp500);
            NvmCacheSet(CACHEKEY_MARKETS_SP500_PCT, sp500PctChange);
        }
        NvmCacheSet(CACHEKEY_LAST_NET, rtc.now().unixtime());
    }

    float temperature, humidity, pressure;
    ReadBME280(&temperature, &humidity, &pressure);
    // Convert to hPa and adjust for 100m of altitude.
    pressure = pressure * 10 + 12;

    DateTime now = rtc.now();
    float d23231_temperature = rtc.getTemperature();

    float battery_voltage = BatteryVoltage();

    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        DrawDate(now);
        DrawClock(120, 120, now);

        // DrawGauge(260, 60, "Work Day", now.hour(), 8, 17);
        // DrawGauge(310, 60, "Work Week", now.dayOfTheWeek(), 1, 5);
        // DrawGauge(360, 110, "Clock Tmp", d23231_temperature, 10, 40);

        // DrawGauge(260, 60, "Month", now.day(), 1, 31);
        DrawGauge(260, 60, "Pressure", pressure, 1000, 1026.5);
        DrawGauge(360, 60, "Battery", battery_voltage, 3.3, 4.2);

        DrawGauge(260, 110, "Inside Tmp", temperature, 10, 40);
        DrawChoice(310, 110, temperature, (now.hour() < 22) ? 23 : 18,
                   outsideTemperature);
        DrawGauge(360, 110, "Outside Tmp", outsideTemperature, 10, 40);

        DrawGauge(260, 160, "Ozone", ozone, 0, 200);
        DrawChoice(310, 160, 100, 0, max(pm25, ozone));
        DrawGauge(360, 160, "PM 2.5", pm25, 0, 200);

        DrawGauge(260, 210, "Inside RH%", humidity, 0, 100);
        DrawGauge(360, 210, "Outside RH%", outsideHumidity, 0, 100);

        // These constants will get old fast... :-)
        DrawGauge(260, 260, "Bitcoin", btc, 7000, 13000);
        DrawGauge(310, 260, "GOOG %", googPctChange * 100, -5, 5);
        DrawGauge(360, 260, "S&P 500 %", sp500PctChange * 100, -5, 5);
    } while (display.nextPage());
    display.hibernate();

    // Signal power off, toggling it a few times to get the TPL5111's attention.
    for (int i = 0; i < 10; i++) {
        digitalWrite(POWER_OFF, HIGH);
        delay(1);
        digitalWrite(POWER_OFF, LOW);
        delay(1);
    }

    // In case we don't power off, wait at least 3 minutes before redrawing.
    // LowPower.deepSleep(180000);
    delay(180000);
}
