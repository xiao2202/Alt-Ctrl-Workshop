#include "Wire.h"           // Library for I2C (How the sensor talks to Arduino)
#include <MPU6050_light.h>  // The helper library that does the hard math

MPU6050 mpu(Wire);          // Create an object named 'mpu'
unsigned long timer = 0;    // A stopwatch to control how fast we send data

void setup() {
  Serial.begin(115200);     // Set the speaking speed (must match Unity!)
  Wire.begin();             // Start the communication
  
  // Start the sensor
  byte status = mpu.begin();
  if(status != 0) {         // If status isn't 0, something is unplugged
    while(1) { 
      Serial.println("Check wiring!"); 
      delay(1000); 
    }
  }
  
  //The Calibration 
  Serial.println("CALIBRATING... Place sensor on a flat table. Do not move it!");
  delay(2000);              // give it a moment to calibrate
  mpu.calcOffsets();        // Calculate the Zero Point to stop the drifting
  Serial.println("Calibration Complete!");
}

void loop() {
  mpu.update();             // Ask the sensor for the latest motion data

  // Send data every 20 milliseconds (50 times per second)
  // This keeps the Unity movement smooth without overwhelming the computer
  if((millis() - timer) > 20) {
    
    // Get the angles from the sensor
    float roll  = mpu.getAngleX();
    float pitch = mpu.getAngleY();
    float yaw   = mpu.getAngleZ();

    // Get the Acceleration (Movement)
    // This tells us how fast the sensor is moving UP or DOWN
    float accZ  = mpu.getAccZ();

    // Send the data to Unity in a comma separated format
    Serial.print(roll);
    Serial.print(",");
    Serial.print(pitch);
    Serial.print(",");
    Serial.print(yaw);
    Serial.print(",");
    Serial.println(accZ);    // println means end of message (New Line)

    timer = millis();       // Reset our stopwatch
  }
}