#include "global.h"

void InitDS3231() {
    // Initialize the real-time clock: DS3231.
    if (!rtc.begin()) {
        Serial.println("RTC not found.");
        Serial.flush();
        abort();
    }
    if (rtc.lostPower()) {
        Serial.println("RTC lost power");
    }
}

float BatteryVoltage() {
    float raw = analogRead(VBATPIN);
    // 50% voltage divider with a 3.3V reference and 1024 divisions.
    return raw * 2.0 * 3.3 / 1024;
}

void InitBME280() {
    //	Wire.begin();
    // begin() checks the Interface, reads the sensor ID (to differentiate
    // between BMP280 and BME280) and reads compensation parameters.
    if (!bmx280.begin()) {
        Serial.println(
            "begin() failed. check your BMx280 Interface and I2C Address.");
        return;
    }

    bmx280.resetToDefaults();
    bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
    bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
}

void ReadBME280(float *temperature, float *humidity, float *pressure) {
    if (!bmx280.measure()) {
        Serial.println("BME280 measurement failure");
        return;
    }
    do {
        delay(100);
    } while (!bmx280.hasValue());

    *temperature = bmx280.getTemperature();
    *humidity = bmx280.getHumidity();
    *pressure = bmx280.getPressure() / 1000;
}

// void InitSGP30() {
//     Serial.println("SGP30 test");

//     if (!sgp.begin()) {
//         Serial.println("Sensor not found :(");
//         return;
//     }
//     Serial.print("Found SGP30 serial #");
//     Serial.print(sgp.serialnumber[0], HEX);
//     Serial.print(sgp.serialnumber[1], HEX);
//     Serial.println(sgp.serialnumber[2], HEX);
// }

// void ReadSGP30() {
//     // If you have a temperature / humidity sensor, you can set the absolute
//     // humidity to enable the humditiy compensation for the air quality
//     signals
//     // float temperature = 22.1; // [°C]
//     // float humidity = 45.2; // [%RH]
//     // sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

//     if (!sgp.IAQmeasure()) {
//         Serial.println("Measurement failed");
//         return;
//     }
//     Serial.print("TVOC ");
//     Serial.print(sgp.TVOC);
//     Serial.print(" ppb\t");
//     Serial.print("eCO2 ");
//     Serial.print(sgp.eCO2);
//     Serial.println(" ppm");

//     if (!sgp.IAQmeasureRaw()) {
//         Serial.println("Raw Measurement failed");
//         return;
//     }
//     Serial.print("Raw H2 ");
//     Serial.print(sgp.rawH2);
//     Serial.print(" \t");
//     Serial.print("Raw Ethanol ");
//     Serial.print(sgp.rawEthanol);
//     Serial.println("");

//     // delay(1000);

//     // counter++;
//     // if (counter == 30) {
//     //     counter = 0;

//     //     uint16_t TVOC_base, eCO2_base;
//     //     if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
//     //         Serial.println("Failed to get baseline readings");
//     //         return;
//     //     }
//     //     Serial.print("****Baseline values: eCO2: 0x");
//     //     Serial.print(eCO2_base, HEX);
//     //     Serial.print(" & TVOC: 0x");
//     //     Serial.println(TVOC_base, HEX);
//     // }
// }