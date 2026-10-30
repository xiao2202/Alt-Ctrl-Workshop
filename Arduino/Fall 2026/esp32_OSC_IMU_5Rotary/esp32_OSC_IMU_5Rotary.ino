/*
 * Alt.Ctrl — BNO055 + 5 Rotary Encoders -> Unreal over OSC
 * Board: FireBeetle 2 ESP32-S3
 * Network: your phone's hotspot (not school Wi-Fi)
 *
 * Sends TWO OSC messages (both floats):
 *   /imu     -> yaw, pitch, roll, accZ         (from the BNO055)
 *   /rotary  -> angle1..angle5 (0-360 degrees) (from the 5 encoders)
 *
 * ---------------- WIRING ----------------
 * BNO055 (I2C):
 *   Vin -> 3V3,  GND -> GND,  SDA -> SDA (IO1),  SCL -> SCL (IO2)
 *
 * Each rotary encoder (VCC, GND, A, B, C):
 *   VCC -> 3V3   (use 3V3, NOT 5V)
 *   GND -> GND
 *   A   -> its A pin below
 *   B   -> its B pin below
 *   C   -> leave unconnected (button not used)
 *
 *   Encoder 1:  A->IO4,  B->IO5
 *   Encoder 2:  A->IO6,  B->IO7
 *   Encoder 3:  A->IO8,  B->IO9
 *   Encoder 4:  A->IO10, B->IO11
 *   Encoder 5:  A->IO14, B->IO21
 *
 * IDE SETTING: Tools -> USB CDC On Boot -> Enabled
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
#include <ESP32Encoder.h>

// --- Your phone hotspot details (CHANGE THESE) ---
const char* ssid     = "iPhone";      // no punctuation in the name
const char* password = "66668888";

// --- Where to send the OSC data (your laptop running Unreal) ---
IPAddress laptopIP(172, 20, 10, 2);   // your laptop's IP on the hotspot (CHANGE THIS)
const int laptopPort = 8000;          // must match Unreal

// --- How many encoder counts make one full 360 turn ---
// EC11 is usually ~20 detents/turn. Turn one full circle, watch the count, adjust.
const float COUNTS_PER_REV = 20.0;

WiFiUDP Udp;

// --- The BNO055 sensor ---
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
bool bnoReady = false;                // did the sensor start OK?

// --- The 5 rotary encoders ---
ESP32Encoder enc1;
ESP32Encoder enc2;
ESP32Encoder enc3;
ESP32Encoder enc4;
ESP32Encoder enc5;

// Turn a raw encoder count into a wrapped 0-360 angle
float countToAngle(long count) {
  float angle = (count / COUNTS_PER_REV) * 360.0;
  angle = fmod(angle, 360.0);
  if (angle < 0) angle += 360.0;
  return angle;
}

void setup() {
  Serial.begin(115200);
  delay(1000);                        // give USB serial a moment to come up
  Serial.println();
  Serial.println("Starting up...");

  // --- Start the BNO055 (does NOT freeze the board if it fails) ---
  Wire.begin();
  if (bno.begin()) {
    bnoReady = true;
    Serial.println("BNO055 OK.");
  } else {
    bnoReady = false;
    Serial.println("WARNING: BNO055 not found. Check wiring. Continuing anyway.");
  }

  // --- Start the 5 encoders (prints after each so you can spot a bad pin) ---
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  Serial.println("enc1..."); enc1.attachHalfQuad(4, 5);
  Serial.println("enc2..."); enc2.attachHalfQuad(6, 7);
  Serial.println("enc3..."); enc3.attachHalfQuad(8, 9);
  Serial.println("enc4..."); enc4.attachHalfQuad(10, 11);
  //Serial.println("enc5..."); enc5.attachHalfQuad(14, 38);
  enc1.clearCount();
  enc2.clearCount();
  enc3.clearCount();
  enc4.clearCount();
  enc5.clearCount();
  Serial.println("Encoders ready.");

  // --- Connect to the phone hotspot (stops trying after ~10s) ---
  Serial.print("Connecting to hotspot: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("SUCCESS! Connected to hotspot.");
    Serial.print("My IP is: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WARNING: could not connect to hotspot.");
    Serial.println("Check the name, password, and that the hotspot is on (2.4GHz).");
    Serial.println("Encoders still work; OSC just won't reach Unreal until connected.");
  }

  Serial.println("Running.");
}

void loop() {
  // ---------- BNO055 (only if it started OK) ----------
  float yaw = 0, pitch = 0, roll = 0, accZ = 0;

  if (bnoReady) {
    sensors_event_t orientationData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    yaw   = orientationData.orientation.x;
    pitch = orientationData.orientation.y;
    roll  = orientationData.orientation.z;

    imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    accZ = accel.z();
  }

  // ---------- Rotary encoders -> 0-360 angles ----------
  float a1 = countToAngle(enc1.getCount());
  float a2 = countToAngle(enc2.getCount());
  float a3 = countToAngle(enc3.getCount());
  float a4 = countToAngle(enc4.getCount());
  float a5 = countToAngle(enc5.getCount());

  // ---------- Send OSC only if Wi-Fi is connected ----------
  if (WiFi.status() == WL_CONNECTED) {
    // /imu
    OSCMessage imuMsg("/imu");
    imuMsg.add(yaw);
    imuMsg.add(pitch);
    imuMsg.add(roll);
    imuMsg.add(accZ);
    Udp.beginPacket(laptopIP, laptopPort);
    imuMsg.send(Udp);
    Udp.endPacket();
    imuMsg.empty();

    // /rotary
    OSCMessage rotMsg("/rotary");
    rotMsg.add(a1);
    rotMsg.add(a2);
    rotMsg.add(a3);
    rotMsg.add(a4);
    rotMsg.add(a5);
    Udp.beginPacket(laptopIP, laptopPort);
    rotMsg.send(Udp);
    Udp.endPacket();
    rotMsg.empty();
  }

  // ---------- Print to Serial Monitor (always, so you can debug) ----------
  Serial.print("IMU: ");
  Serial.print(yaw); Serial.print(", ");
  Serial.print(pitch); Serial.print(", ");
  Serial.print(roll); Serial.print(", ");
  Serial.print(accZ);
  Serial.print("  |  ROTARY: ");
  Serial.print(a1); Serial.print(", ");
  Serial.print(a2); Serial.print(", ");
  Serial.print(a3); Serial.print(", ");
  Serial.print(a4); Serial.print(", ");
  Serial.println(a5);

  delay(50);   // ~20 times per second
}