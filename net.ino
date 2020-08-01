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
    HttpClient client = HttpClient(wifi, "worldtimeapi.org", 80);
    Serial.println("Requesting time from worldtimeapi.org...");
    client.get("/api/ip");

    int statusCode = client.responseStatusCode();
    if (statusCode != 200) {
        Serial.print("HTTP request failed to worldtimeapi.org: ");
        Serial.println(statusCode);
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, client.responseBody());
    if (error) {
        Serial.print("Could not parse time from worldtimeapi.org: ");
        Serial.println(error.c_str());
        return;
    }

    // Extract time and adjust the clock.
    DateTime newTime(doc["datetime"].as<char*>());
    DateTime oldTime = rtc.now();
    rtc.adjust(doc["datetime"].as<char*>());

    Serial.print("Adjusted the time by ");
    Serial.print((newTime - oldTime).seconds());
    Serial.print(" seconds to: ");
    char timeBuffer[] = "YY-MM-DD hh:mm:ss AP";
    Serial.println(newTime.toString(timeBuffer));
}
