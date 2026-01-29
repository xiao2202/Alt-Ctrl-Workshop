#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

int buttonPin = 2;
int potPin = A0;

float pitch = 0.0;
float roll = 0.0;
float yaw = 0.0;

unsigned long lastTime = 0;

void setup(void) {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  if (!mpu.begin()) {
    //Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  //Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
  lastTime = millis();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0; 
  lastTime = currentTime;

  // Calculate accelerometer angles (in degrees)
  float accelPitch = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180 / PI;
  float accelRoll = atan2(-a.acceleration.x, a.acceleration.z) * 180 / PI;

  // Integrate gyroscope data (convert rad/s to deg/s)
  float gyroPitchRate = g.gyro.x * 180 / PI;
  float gyroRollRate = g.gyro.y * 180 / PI;
  float gyroYawRate = g.gyro.z * 180 / PI;

  // Complementary filter
  const float alpha = 0.98; 
  pitch = alpha * (pitch + gyroPitchRate * dt) + (1 - alpha) * accelPitch;
  roll = alpha * (roll + gyroRollRate * dt) + (1 - alpha) * accelRoll;
  yaw += gyroYawRate * dt; 

  int buttonVal = digitalRead(buttonPin);
  int potVal = analogRead(potPin);
  // --- Print values ---
  Serial.print(buttonVal);
  Serial.print(",");
  Serial.print(potVal);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.print(roll);
  Serial.print(",");
  Serial.println(yaw);

  delay(50);
}

