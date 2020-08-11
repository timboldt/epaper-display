#include "global.h"

void MountFilesystem() {
    // Initialize flash library and check its chip ID.
    if (!flash.begin()) {
        Serial.println("Error, failed to initialize flash chip!");
        return;
    }
    Serial.print("Flash chip JEDEC ID: 0x");
    Serial.println(flash.getJEDECID(), HEX);

    // First call begin to mount the filesystem.  Check that it returns true
    // to make sure the filesystem was mounted.
    if (!fatfs.begin(&flash)) {
        Serial.println("Error, failed to mount newly formatted filesystem!");
        Serial.println(
            "Was the flash chip formatted with the fatfs_format example?");
        return;
    }
    Serial.println("Mounted filesystem!");
}

bool ReadFromCache(String filename, JsonDocument &doc, float seconds) {
    File file = fatfs.open(filename, FILE_READ);
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
        Serial.print("Failed to parse file: ");
        Serial.println(filename);
        return false;
    }
    float cacheTime = doc["cache-timestamp"].as<float>();
    bool valid = float(rtc.now().unixtime()) - cacheTime <= seconds;
    if (valid) {
        Serial.print("Found doc in cache: ");
        Serial.println(filename);
    } else {
        Serial.print("Now: ");
        Serial.print(rtc.now().unixtime());
        Serial.print(" Cache: ");
        Serial.println(cacheTime);
    }
    return valid;
}

void SaveDocToCache(String filename, JsonDocument &doc) {
    Serial.print("Saving to cache: ");
    Serial.println(filename);
    fatfs.remove(filename.c_str());
    File file = fatfs.open(filename, FILE_WRITE);
    doc["cache-timestamp"] = float(rtc.now().unixtime());
    serializeJson(doc, file);
    file.close();
}