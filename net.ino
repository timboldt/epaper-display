#include <ArduinoJson.h>
#include <HttpClient.h>
#include <WiFiNINA.h>

#include "wifi_secrets.h"

WiFiClient wifi;

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

    SetTimeFromWeb();

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
    Serial.println("making GET request");
    client.get("/api/ip");

    // read the status code and body of the response
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    // Extract values
    Serial.println(F("Response:"));
    Serial.println(doc["client_ip"].as<char*>());
    Serial.println(doc["datetime"].as<char*>());
    rtc.adjust(doc["datetime"].as<char*>());
}
