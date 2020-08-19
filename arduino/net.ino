#include "global.h"

bool ConnectToNetwork() {
    WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
    while (WiFi.status() == WL_NO_MODULE) {
        Serial.println("WiFi: Communication with Airlift module failed.");
        return false;
    }
    String fv = WiFi.firmwareVersion();
    Serial.print("WiFi firmware version: ");
    Serial.println(fv);

    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("WiFi MAC address: ");
    PrintMacAddress(mac);

    WiFi.lowPowerMode();

    int status = WiFi.begin(SECRET_SSID, SECRET_PASS);
    if (status != WL_CONNECTED) {
        Serial.print("WiFi connection failed: ");
        Serial.println(status);
        return false;
    }
    Serial.print("WiFi SSID: ");
    Serial.println(WiFi.SSID());

    IPAddress ip = WiFi.localIP();
    Serial.print("WiFi IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("WiFi RSSI:");
    Serial.print(rssi);
    Serial.println(" dBm");

    return true;
}

void DisconnectFromNetwork() { WiFi.end(); }

void PrintMacAddress(byte mac[]) {
    for (int i = 5; i >= 0; i--) {
        if (mac[i] < 16) {
            Serial.print("0");
        }
        Serial.print(mac[i], HEX);
        if (i > 0) {
            Serial.print(":");
        }
    }
    Serial.println();
}

void SetTimeFromWeb() {
    const int oneDay = 3600 * 24;
    DynamicJsonDocument doc(1024);
    if (!ReadFromCache("time.json", doc, oneDay)) {
        if (ConnectToNetwork()) {
            HttpClient client = HttpClient(wifi, "worldtimeapi.org", 80);
            Serial.println("Requesting time from worldtimeapi.org...");
            client.get("/api/ip");

            int statusCode = client.responseStatusCode();
            if (statusCode != 200) {
                // TODO: Clean up the network logic instead of having messy exit
                // paths.
                DisconnectFromNetwork();
                Serial.print("HTTP request failed to worldtimeapi.org: ");
                Serial.println(statusCode);
                return;
            }

            DynamicJsonDocument doc(1024);
            DeserializationError error =
                deserializeJson(doc, client.responseBody());
            DisconnectFromNetwork();
            if (error) {
                Serial.print("Could not parse time from worldtimeapi.org: ");
                Serial.println(error.c_str());
                return;
            }

            // Extract time and adjust the clock.
            DateTime newTime(doc["datetime"].as<char *>());
            DateTime oldTime = rtc.now();
            rtc.adjust(doc["datetime"].as<char *>());

            Serial.print("Adjusted the time by ");
            Serial.print((newTime - oldTime).seconds());
            Serial.print(" seconds to: ");
            char timeBuffer[] = "YY-MM-DD hh:mm:ss AP";
            Serial.println(newTime.toString(timeBuffer));

            SaveDocToCache("time.json", doc);
        }
    }
}

void GetWeatherFromWeb(float *temperature, float *humidity) {
    const char cacheFile[] = "weather.json";
    const int oneHour = 3600;
    DynamicJsonDocument doc(1024);
    if (!ReadFromCache(cacheFile, doc, oneHour)) {
        if (ConnectToNetwork()) {
            HttpClient client = HttpClient(wifi, "api.openweathermap.org", 80);
            Serial.println("Requesting weather from api.openweathermap.org...");
            client.get(
                "/data/2.5/"
                "weather?id=5383777&units=metric&"
                "appid=" OPENWEATHER_API_KEY);

            int statusCode = client.responseStatusCode();
            if (statusCode != 200) {
                DisconnectFromNetwork();
                Serial.print("HTTP request failed to api.openweathermap.org: ");
                Serial.println(statusCode);
                return;
            }
            DeserializationError error =
                deserializeJson(doc, client.responseBody());
            DisconnectFromNetwork();

            if (error) {
                Serial.print(
                    "Could not parse time from "
                    "api.openweathermap.org: ");
                Serial.println(error.c_str());
                return;
            }
            SaveDocToCache(cacheFile, doc);
        }
    }

    *temperature = doc["main"]["temp"].as<float>();
    *humidity = doc["main"]["humidity"].as<float>();
}

void GetAQIFromWeb(float *o3, float *pm25) {
    const char cacheFile[] = "aqi.json";
    const int oneHour = 3600;
    DynamicJsonDocument doc(1024);
    if (!ReadFromCache(cacheFile, doc, oneHour)) {
        if (ConnectToNetwork()) {
            HttpClient client = HttpClient(wifi, "www.airnowapi.org", 80);
            Serial.println("Requesting air quality from AirNow...");
            client.get(
                "/aq/observation/zipCode/current/?format=application/json"
                "&zipCode=94566&distance=25&API_KEY=" AIRNOW_API_KEY);

            int statusCode = client.responseStatusCode();
            if (statusCode != 200) {
                DisconnectFromNetwork();
                Serial.print("HTTP request failed to AirNow: ");
                Serial.println(statusCode);
                return;
            }
            DeserializationError error =
                deserializeJson(doc, client.responseBody());
            DisconnectFromNetwork();

            if (error) {
                Serial.print("Could not parse data from AirNow: ");
                Serial.println(error.c_str());
                return;
            }
            SaveDocToCache(cacheFile, doc);
        }
    }

    // TODO: Verify that doc order is okay. If not look at "parameter".
    *o3 = doc[0]["AQI"].as<float>();
    *pm25 = doc[1]["AQI"].as<float>();
}

void GetBTCFromWeb(float *btc) {
    const char cacheFile[] = "btc.json";
    const int oneHour = 3600;
    DynamicJsonDocument doc(1024);
    if (!ReadFromCache(cacheFile, doc, oneHour)) {
        if (ConnectToNetwork()) {
            HttpClient client = HttpClient(wifi, "api.coindesk.com", 80);
            Serial.println("Requesting BTC price from Coindesk...");
            client.get("/v1/bpi/currentprice/USD.json");

            int statusCode = client.responseStatusCode();
            if (statusCode != 200) {
                DisconnectFromNetwork();
                Serial.print("HTTP request failed to Coindesk: ");
                Serial.println(statusCode);
                return;
            }
            DeserializationError error =
                deserializeJson(doc, client.responseBody());
            DisconnectFromNetwork();

            if (error) {
                Serial.print("Could not parse data from Coindesk: ");
                Serial.println(error.c_str());
                return;
            }
            SaveDocToCache(cacheFile, doc);
        }
    }

    *btc = doc["bpi"]["USD"]["rate_float"].as<float>();
}
