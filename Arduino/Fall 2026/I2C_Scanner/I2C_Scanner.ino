/*
 * I2C Scanner — checks if the BNO055 is alive and finds its address
 * Board: FireBeetle 2 ESP32-S3
 *
 * Upload this, open Serial Monitor at 115200, and read the result:
 *   - Sees 0x28 or 0x29  -> sensor is ALIVE, wiring is good
 *   - Sees nothing       -> wiring problem or dead sensor
 */

#include <Wire.h>              // I2C library

void setup() {
  Serial.begin(115200);       // talk to Serial Monitor
  Wire.begin();               // start I2C on default SDA/SCL pins
  delay(1000);
  Serial.println("Scanning for I2C devices...");
}

void loop() {
  int found = 0;              // count how many devices we find

  // Check every possible I2C address (1 to 126)
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);   // try to talk to this address
    byte error = Wire.endTransmission(); // 0 means a device answered

    if (error == 0) {                  // something is there!
      Serial.print("Found device at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);    // print its address in hex
      found++;
    }
  }

  if (found == 0) {
    Serial.println("No devices found. Check wiring.");
  }

  Serial.println("---");
  delay(2000);               // scan again every 2 seconds
}