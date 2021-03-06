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

bool ReadFromCache(String contentFileName, JsonDocument &contentDoc,
                   float seconds) {
    String cacheFileName = "cache_" + contentFileName;

    // Read and parse the cache file.
    File cacheFile = fatfs.open(cacheFileName, FILE_READ);
    DynamicJsonDocument cacheDoc(1024);
    DeserializationError error = deserializeJson(cacheDoc, cacheFile);
    cacheFile.close();
    if (error) {
        Serial.print("Failed to parse cache file: ");
        Serial.println(cacheFileName);
        Serial.println(error.c_str());
        return false;
    }

    // Read and parse the content file.
    File contentFile = fatfs.open(contentFileName, FILE_READ);
    error = deserializeJson(contentDoc, contentFile);
    contentFile.close();
    if (error) {
        Serial.print("Failed to parse file: ");
        Serial.println(contentFileName);
        Serial.println(error.c_str());
        return false;
    }

    float cacheTime = cacheDoc["cache-timestamp"].as<float>();
    Serial.print("Found cached file : ");
    Serial.println(contentFileName);
    Serial.print("Now: ");
    Serial.print(rtc.now().unixtime());
    Serial.print(" Cache time: ");
    Serial.println(cacheTime);

    return float(rtc.now().unixtime()) - cacheTime <= seconds;
}

void SaveDocToCache(String contentFileName, JsonDocument &contentDoc) {
    // Write the file.
    Serial.print("Saving to cache: ");
    Serial.println(contentFileName);
    fatfs.remove(contentFileName.c_str());
    File file = fatfs.open(contentFileName, FILE_WRITE);
    serializeJson(contentDoc, file);
    file.close();

    // Write the time in the cache file.
    String cacheFileName = "cache_" + contentFileName;
    fatfs.remove(cacheFileName.c_str());
    File cacheFile = fatfs.open(cacheFileName, FILE_WRITE);
    DynamicJsonDocument cacheDoc(1024);
    cacheDoc["cache-timestamp"] = float(rtc.now().unixtime());
    serializeJson(cacheDoc, cacheFile);
    cacheFile.close();
}

void NvmCacheInit() {
    const float magicValue = 1234.5f;
    float isValid = NvmCacheGet(CACHEKEY_IS_VALID);
    if (isValid != magicValue) {
        Serial.println("Initializing FRAM cache.");
        for (int i = 0; i < CACHEKEY_NUM_KEYS; i++) {
            NvmCacheSet(CACHEKEY_IS_VALID, 0.0f);
        }
        NvmCacheSet(CACHEKEY_IS_VALID, magicValue);
    }

    NvmCacheDump();
}

void NvmCacheDump() {
    for (int i = 0; i < CACHEKEY_NUM_KEYS; i++) {
        Serial.print("Cache[");
        Serial.print(i);
        Serial.print("]: ");
        Serial.println(NvmCacheGet(i));
    }
}

float NvmCacheGet(int key) {
    auto offset = key * 4;
    uint32_t raw = fram.read8(offset) | (fram.read8(offset + 1) << 8) |
                   (fram.read8(offset + 2) << 16) |
                   (fram.read8(offset + 3) << 24);
    return *(float *)&raw;
}

void NvmCacheSet(int key, float value) {
    auto offset = key * 4;
    uint32_t raw = *(uint32_t *)&value;
    fram.write8(offset, raw & 0xff);
    fram.write8(offset + 1, (raw >> 8) & 0xff);
    fram.write8(offset + 2, (raw >> 16) & 0xff);
    fram.write8(offset + 3, raw >> 24);
}
