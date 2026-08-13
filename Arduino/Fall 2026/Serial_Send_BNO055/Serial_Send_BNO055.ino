#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// BNO055 over I2C (default address 0x28)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
unsigned long timer = 0;

void setup() {
  Serial.begin(115200);   // ESP32 handles higher baud easily; match this in Unity

  // FireBeetle 2 ESP32-S3 default I2C: SDA=1, SCL=2
  // If your board/breakout differs, change these two pins
  Wire.begin(1, 2);

  if (!bno.begin()) {
    while (1) {
      Serial.println("Check wiring! BNO055 not detected.");
      delay(1000);
    }
  }

  Serial.println("CALIBRATING... Move sensor slowly in figure-8, then rest on a flat table.");
  delay(2000);

  bno.setExtCrystalUse(true);  // use external crystal on the Adafruit board for better timing

  Serial.println("Ready!");
}

void loop() {
  if ((millis() - timer) > 20) {  // ~50 Hz

    // Fused absolute orientation (Euler angles, degrees)
    sensors_event_t orientationData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

    float yaw   = orientationData.orientation.x;  // heading
    float pitch = orientationData.orientation.y;
    float roll  = orientationData.orientation.z;

    // Linear acceleration (gravity removed), m/s^2
    imu::Vector<3> linAcc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    float accZ = linAcc.z();

    // Same CSV format your Unity parser already expects: roll,pitch,yaw,accZ
    Serial.print(roll);
    Serial.print(",");
    Serial.print(pitch);
    Serial.print(",");
    Serial.print(yaw);
    Serial.print(",");
    Serial.println(accZ);

    timer = millis();
  }
}
