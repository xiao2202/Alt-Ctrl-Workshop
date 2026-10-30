/*
 * Alt.Ctrl Workshop — Send BNO055 data to Unreal Engine over OSC
 * Board: FireBeetle 2 ESP32-S3
 * Network: your phone's hotspot (not school Wi-Fi)
 *
 * WIRING (BNO055 to FireBeetle):
 *   BNO055 Vin -> 3V3     (power)
 *   BNO055 GND -> GND     (ground)
 *   BNO055 SDA -> SDA     (I2C data)
 *   BNO055 SCL -> SCL     (I2C clock)
 *
 * IDE SETTING: Tools -> USB CDC On Boot -> Enabled
 * AFTER UPLOAD: if you see only "ESP-ROM:esp32s3..." press the RST button once.
 *
 * LIBRARIES to install (Library Manager):
 *   - Adafruit BNO055
 *   - Adafruit Unified Sensor
 *   - OSC (by Adrian Freed / CNMAT)
 */

#include <WiFi.h>                    // lets the ESP32 join a Wi-Fi network
#include <WiFiUdp.h>                 // sends data over the network using UDP
#include <OSCMessage.h>             // builds OSC messages
#include <Wire.h>                    // lets the board talk to I2C sensors
#include <Adafruit_Sensor.h>        // base library the BNO055 needs
#include <Adafruit_BNO055.h>        // the BNO055 sensor library

// --- Your phone hotspot details (CHANGE THESE) ---
const char* ssid     = "YOUR_HOT_SPOT_NAME";      // your phone hotspot's name, no punctuation
const char* password = "YOUR_PASSWORD";   // your phone hotspot's password

// --- Where to send the OSC data (your laptop running Unreal) ---
IPAddress laptopIP(172, 100, 20, 10);   // your laptop's IP on the hotspot (CHANGE THIS)
const int laptopPort = 8001;             // the port Unreal listens on (must match Unreal)

WiFiUDP Udp;                          // the tool that actually sends the UDP packets

// --- Create the sensor object (55 is just an ID, 0x28 is its address) ---
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
bool bnoReady = false;                // did the sensor start OK?

void setup() {
  Serial.begin(115200);              // start talking to the Serial Monitor
  delay(1000);                       // give USB serial a moment to come up
  Serial.println();
  Serial.println("Starting up...");

  // Start the I2C connection on the FireBeetle's default SDA/SCL pins
  Wire.begin();

  // Start the sensor; if it fails, WARN but keep going (do NOT freeze)
  if (bno.begin()) {
    bnoReady = true;
    Serial.println("BNO055 OK.");
  } else {
    bnoReady = false;
    Serial.println("WARNING: BNO055 not found. Check wiring. Continuing anyway.");
  }

  // Connect to the phone hotspot (gives up after ~10s instead of hanging)
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
    Serial.println("Sensor still prints; OSC just won't reach Unreal until connected.");
  }

  Serial.println("Running.");
}

void loop() {
  // Read the sensor only if it started OK
  float yaw = 0, pitch = 0, roll = 0, accZ = 0;

  if (bnoReady) {
    sensors_event_t orientationData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

    yaw   = orientationData.orientation.x;  // heading / turning
    pitch = orientationData.orientation.y;  // tilt forward/back
    roll  = orientationData.orientation.z;  // tilt left/right

    // Read acceleration with gravity removed (linear acceleration)
    imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    accZ = accel.z();                       // up/down acceleration
  }

  // --- Send OSC only if Wi-Fi is connected ---
  if (WiFi.status() == WL_CONNECTED) {
    OSCMessage msg("/imu");          // "/imu" is the address Unreal listens for
    msg.add(yaw);                    // 1st value
    msg.add(pitch);                  // 2nd value
    msg.add(roll);                   // 3rd value
    msg.add(accZ);                   // 4th value

    Udp.beginPacket(laptopIP, laptopPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
  }

  // Print to Serial Monitor so you can see it working (always)
  Serial.print(yaw); Serial.print(", ");
  Serial.print(pitch); Serial.print(", ");
  Serial.print(roll); Serial.print(", ");
  Serial.println(accZ);

  delay(50);                          // send about 20 times per second
}