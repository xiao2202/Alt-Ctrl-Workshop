/*
 * Alt.Ctrl — BNO055 + 5 Rotary Encoders -> Unreal over OSC
 * Board: FireBeetle 2 ESP32-S3
 * Network: your phone's hotspot (not school Wi-Fi)
 *
 * Sends TWO kinds of OSC message:
 *   /imu     -> yaw, pitch, roll, accZ   (from the BNO055)
 *   /rotary  -> enc1, enc2, enc3, enc4, enc5  (from the 5 encoders)
 *
 * ---------------- WIRING ----------------
 * BNO055 (I2C):
 *   Vin -> 3V3
 *   GND -> GND
 *   SDA -> SDA  (IO1)
 *   SCL -> SCL  (IO2)
 *
 * Each rotary encoder breakout (VCC, A, C, B):
 *   VCC -> 3V3   (use 3V3, NOT 5V, even though the label says 5V)
 *   C   -> GND
 *   A   -> its A pin below
 *   B   -> its B pin below
 *
 *   Encoder 1:  A->IO4,  B->IO5
 *   Encoder 2:  A->IO6,  B->IO7
 *   Encoder 3:  A->IO8,  B->IO9
 *   Encoder 4:  A->IO10, B->IO11
 *   Encoder 5:  A->IO12, B->IO13
 *
 *   All VCC pins share 3V3. All C pins share GND. (A common 3V3 rail
 *   and a common GND rail make this much tidier.)
 *
 * LIBRARIES (Library Manager):
 *   - Adafruit BNO055
 *   - Adafruit Unified Sensor
 *   - OSC (by CNMAT)
 *   - ESP32Encoder (by Kevin Harrington)
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <ESP32Encoder.h>           // handles reading the rotary encoders

// --- Your phone hotspot details (CHANGE THESE) ---
const char* ssid     = "YOUR_HOTSPOT_NAME";      // no punctuation in the name
const char* password = "YOUR_HOTSPOT_PASSWORD";

// --- Where to send the OSC data (your laptop running Unreal) ---
IPAddress laptopIP(172, 20, 10, 2);   // your laptop's IP on the hotspot (CHANGE THIS)
const int laptopPort = 8000;          // must match Unreal

WiFiUDP Udp;

// --- The BNO055 sensor ---
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

// --- The 5 rotary encoders ---
ESP32Encoder enc1;
ESP32Encoder enc2;
ESP32Encoder enc3;
ESP32Encoder enc4;
ESP32Encoder enc5;

void setup() {
  Serial.begin(115200);

  // Start I2C for the BNO055 (SDA=IO1, SCL=IO2 are the FireBeetle defaults)
  Wire.begin();

  if (!bno.begin()) {
    Serial.println("BNO055 not found! Check wiring.");
    while (1);
  }

  // Start each encoder on its two pins (A, B).
  // useInternalWeakPullResistors turns on pull-ups so idle pins read cleanly.
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  enc1.attachHalfQuad(4, 5);
  enc2.attachHalfQuad(6, 7);
  enc3.attachHalfQuad(8, 9);
  enc4.attachHalfQuad(10, 11);
  enc5.attachHalfQuad(12, 13);

  // Start all encoders counting from zero
  enc1.clearCount();
  enc2.clearCount();
  enc3.clearCount();
  enc4.clearCount();
  enc5.clearCount();

  // Connect to the phone hotspot
  Serial.print("Connecting to hotspot");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");
  Serial.print("My IP is: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // ---------- BNO055 ----------
  sensors_event_t orientationData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

  float yaw   = orientationData.orientation.x;
  float pitch = orientationData.orientation.y;
  float roll  = orientationData.orientation.z;

  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  float accZ = accel.z();

  // Send the IMU message on /imu
  OSCMessage imuMsg("/imu");
  imuMsg.add(yaw);
  imuMsg.add(pitch);
  imuMsg.add(roll);
  imuMsg.add(accZ);

  Udp.beginPacket(laptopIP, laptopPort);
  imuMsg.send(Udp);
  Udp.endPacket();
  imuMsg.empty();

  // ---------- Rotary encoders ----------
  // Read each encoder's accumulated count as a number
  long r1 = enc1.getCount();
  long r2 = enc2.getCount();
  long r3 = enc3.getCount();
  long r4 = enc4.getCount();
  long r5 = enc5.getCount();

  // Send the rotary message on /rotary
  OSCMessage rotMsg("/rotary");
  rotMsg.add((int32_t)r1);
  rotMsg.add((int32_t)r2);
  rotMsg.add((int32_t)r3);
  rotMsg.add((int32_t)r4);
  rotMsg.add((int32_t)r5);

  Udp.beginPacket(laptopIP, laptopPort);
  rotMsg.send(Udp);
  Udp.endPacket();
  rotMsg.empty();

  // Print both to Serial Monitor so you can see them working
  Serial.print("IMU: ");
  Serial.print(yaw); Serial.print(", ");
  Serial.print(pitch); Serial.print(", ");
  Serial.print(roll); Serial.print(", ");
  Serial.print(accZ);
  Serial.print("  |  ROTARY: ");
  Serial.print(r1); Serial.print(", ");
  Serial.print(r2); Serial.print(", ");
  Serial.print(r3); Serial.print(", ");
  Serial.print(r4); Serial.print(", ");
  Serial.println(r5);

  delay(50);   // ~20 times per second
}