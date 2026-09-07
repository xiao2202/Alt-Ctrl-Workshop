/*
 * Nano 33 IoT -> p5.js : stream IMU as roll, pitch, yaw over serial
 * IMU chip: LSM6DS3 (3-axis accel + 3-axis gyro, NO magnetometer)
 *
 * Library: install "Arduino_LSM6DS3" by Arduino (Library Manager)
 *
 * NOTE ON YAW: this board has no magnetometer, so yaw is integrated
 * from the gyroscope and WILL slowly drift. Roll and pitch are solid.
 */

#include <Arduino_LSM6DS3.h>

float yaw = 0;                 // integrated gyro angle (drifts)
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);             // wait for serial (native USB board)

  if (!IMU.begin()) {
    Serial.println("Failed to init IMU!");
    while (1);
  }
  lastTime = millis();
}

void loop() {
  float ax, ay, az;   // accelerometer (g)
  float gx, gy, gz;   // gyroscope (degrees/sec)

  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);

    // Roll and pitch from gravity direction (stable, no drift)
    float roll  = atan2(ay, az) * 180.0 / PI;
    float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

    // Yaw: integrate gyro Z over time (drifts, no absolute reference)
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;
    lastTime = now;
    yaw += gz * dt;

    // Send as CSV: roll,pitch,yaw
    Serial.print(roll, 2);
    Serial.print(",");
    Serial.print(pitch, 2);
    Serial.print(",");
    Serial.println(yaw, 2);
  }
}