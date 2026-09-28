/*
 * Nano 33 IoT -> Unity : send onboard IMU data over serial as roll,pitch,yaw,accZ
 * Onboard IMU chip: LSM6DS3 (3-axis accelerometer + 3-axis gyroscope)
 *
 * IMPORTANT: this chip has NO magnetometer, so it cannot measure absolute yaw.
 * Roll and pitch come from the accelerometer (stable). Yaw is integrated from
 * the gyroscope and WILL slowly drift on its own. This is a hardware limit,
 * not a code bug.
 *
 * Library: install "Arduino_LSM6DS3" by Arduino (Library Manager)
 * Baud: 115200 (must match Unity)
 */

#include <Arduino_LSM6DS3.h>   // the Nano 33 IoT's onboard IMU library

float yaw = 0;                 // yaw is built up over time (drifts)
unsigned long timer = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);        // match this in Unity
  while (!Serial);             // wait for the USB serial connection

  if (!IMU.begin()) {          // start the onboard IMU
    while (1) {
      Serial.println("Check IMU! LSM6DS3 not detected.");
      delay(1000);
    }
  }

  Serial.println("Ready!");
  lastTime = millis();
}

void loop() {
  if ((millis() - timer) > 20) {  // ~50 Hz

    float ax, ay, az;   // accelerometer values (in g)
    float gx, gy, gz;   // gyroscope values (in degrees/second)

    // Only read when both sensors have fresh data
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(ax, ay, az);
      IMU.readGyroscope(gx, gy, gz);

      // Roll and pitch from gravity direction (stable, no drift)
      float roll  = atan2(ay, az) * 180.0 / PI;
      float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

      // Yaw: add up the gyro's turning speed over time (this drifts)
      unsigned long now = millis();
      float dt = (now - lastTime) / 1000.0;   // seconds since last reading
      lastTime = now;
      yaw += gz * dt;                          // spin speed x time = angle change

      // Z acceleration, same slot as before
      float accZ = az;

      // Same CSV format Unity already expects: roll,pitch,yaw,accZ
      Serial.print(roll);
      Serial.print(",");
      Serial.print(pitch);
      Serial.print(",");
      Serial.print(yaw);
      Serial.print(",");
      Serial.println(accZ);
    }

    timer = millis();
  }
}