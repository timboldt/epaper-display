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