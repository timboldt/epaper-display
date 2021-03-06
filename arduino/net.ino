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
    DynamicJsonDocument doc(1024);
    HttpClient client = HttpClient(wifi, "worldtimeapi.org", 80);
    Serial.println("Requesting time from worldtimeapi.org...");
    client.get("/api/ip");

    int statusCode = client.responseStatusCode();
    if (statusCode != 200) {
        // TODO: Clean up the network logic instead of having messy exit
        // paths.
        Serial.print("HTTP request failed to worldtimeapi.org: ");
        Serial.println(statusCode);
        return;
    }

    DeserializationError error = deserializeJson(doc, client.responseBody());
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
}

void GetWeatherFromWeb(float *temperature, float *humidity) {
    DynamicJsonDocument doc(1024);
    HttpClient client = HttpClient(wifi, "api.openweathermap.org", 80);
    Serial.println("Requesting weather from api.openweathermap.org...");
    client.get(
        "/data/2.5/"
        "weather?id=5383777&units=metric&"
        "appid=" OPENWEATHER_API_KEY);

    int statusCode = client.responseStatusCode();
    if (statusCode != 200) {
        Serial.print("HTTP request failed to api.openweathermap.org: ");
        Serial.println(statusCode);
        return;
    }
    DeserializationError error = deserializeJson(doc, client.responseBody());

    if (error) {
        Serial.print(
            "Could not parse time from "
            "api.openweathermap.org: ");
        Serial.println(error.c_str());
        return;
    }

    *temperature = doc["main"]["temp"].as<float>();
    *humidity = doc["main"]["humidity"].as<float>();
}

void GetAQIFromWeb(float *o3, float *pm25) {
    DynamicJsonDocument doc(1024);
    HttpClient client = HttpClient(wifi, "www.airnowapi.org", 80);
    Serial.println("Requesting air quality from AirNow...");
    client.get(
        "/aq/observation/zipCode/current/?format=application/json"
        "&zipCode=94566&distance=25&API_KEY=" AIRNOW_API_KEY);

    int statusCode = client.responseStatusCode();
    if (statusCode != 200) {
        Serial.print("HTTP request failed to AirNow: ");
        Serial.println(statusCode);
        return;
    }
    DeserializationError error = deserializeJson(doc, client.responseBody());

    if (error) {
        Serial.print("Could not parse data from AirNow: ");
        Serial.println(error.c_str());
        return;
    }

    // TODO: Verify that doc order is okay. If not look at "parameter".
    *o3 = doc[0]["AQI"].as<float>();
    *pm25 = doc[1]["AQI"].as<float>();
}

void GetBTCFromWeb(float *btc) {
    DynamicJsonDocument doc(1024);
    HttpClient client = HttpClient(wifi, "api.coindesk.com", 80);
    Serial.println("Requesting BTC price from Coindesk...");
    client.get("/v1/bpi/currentprice/USD.json");

    int statusCode = client.responseStatusCode();
    if (statusCode != 200) {
        Serial.print("HTTP request failed to Coindesk: ");
        Serial.println(statusCode);
        return;
    }
    DeserializationError error = deserializeJson(doc, client.responseBody());

    if (error) {
        Serial.print("Could not parse data from Coindesk: ");
        Serial.println(error.c_str());
        return;
    }

    *btc = doc["bpi"]["USD"]["rate_float"].as<float>();
}

void GetSP500FromWeb(float *sp500, float *sp500PctChange) {
    DynamicJsonDocument doc(1024);
    HttpClient client = HttpClient(wifissl, "www.alphavantage.co", 443);
    Serial.println("Requesting SPY price from Alpha Vantage...");
    client.get(
        "/query?function=GLOBAL_QUOTE&symbol=SPY&"
        "apikey=" ALPHAVANTAGE_API_KEY);

    int statusCode = client.responseStatusCode();
    if (statusCode != 200) {
        Serial.print("HTTP request failed to Alpha Vantage: ");
        Serial.println(statusCode);
        return;
    }
    DeserializationError error = deserializeJson(doc, client.responseBody());

    if (error) {
        Serial.print("Could not parse data from Alpha Vantage: ");
        Serial.println(error.c_str());
        return;
    }

    *sp500 = doc["Global Quote"]["05. price"].as<float>();
    float change = doc["Global Quote"]["09. change"].as<float>();
    *sp500PctChange = change / (*sp500);
}

void GetGOOGFromWeb(float *goog, float *googPctChange) {
    DynamicJsonDocument doc(1024);
    HttpClient client = HttpClient(wifissl, "www.alphavantage.co", 443);
    Serial.println("Requesting GOOG price from Alpha Vantage...");
    client.get(
        "/query?function=GLOBAL_QUOTE&symbol=GOOG&"
        "apikey=" ALPHAVANTAGE_API_KEY);

    int statusCode = client.responseStatusCode();
    if (statusCode != 200) {
        Serial.print("HTTP request failed to Alpha Vantage: ");
        Serial.println(statusCode);
        return;
    }
    DeserializationError error = deserializeJson(doc, client.responseBody());

    if (error) {
        Serial.print("Could not parse data from Alpha Vantage: ");
        Serial.println(error.c_str());
        return;
    }

    *goog = doc["Global Quote"]["05. price"].as<float>();
    float change = doc["Global Quote"]["09. change"].as<float>();
    *googPctChange = change / (*goog);
}
