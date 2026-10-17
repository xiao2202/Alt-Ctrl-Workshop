/*
 * FireBeetle 2 ESP32-S3 + BNO055 -> p5.js : stream roll, pitch, yaw, accZ over serial
 * IMU: BNO055 (onboard fusion + magnetometer, so yaw is STABLE, no drift)
 *
 * WIRING (BNO055 to FireBeetle):
 *   Vin -> 3V3,  GND -> GND,  SDA -> SDA,  SCL -> SCL
 *
 * Libraries (Library Manager):
 *   - Adafruit BNO055
 *   - Adafruit Unified Sensor
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup() {
  Serial.begin(115200);
  while (!Serial);             // wait for serial (native USB board)

  Wire.begin();                // start I2C on default SDA/SCL pins

  if (!bno.begin()) {
    Serial.println("Failed to init BNO055! Check wiring.");
    while (1);
  }
}

void loop() {
  // Get fused orientation in degrees (no math needed, no drift)
  sensors_event_t orientationData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

  float yaw   = orientationData.orientation.x;  // heading
  float pitch = orientationData.orientation.y;
  float roll  = orientationData.orientation.z;

  // Linear acceleration = movement only, gravity already removed (m/s^2)
  imu::Vector<3> linAcc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  float accZ = linAcc.z();

  // Send as CSV: roll,pitch,yaw,accZ
  Serial.print(roll, 2);
  Serial.print(",");
  Serial.print(pitch, 2);
  Serial.print(",");
  Serial.print(yaw, 2);
  Serial.print(",");
  Serial.println(accZ, 2);

  delay(20);   // ~50 Hz
}