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
const char* ssid     = "YOUR_HOTSPOT_NAME";      // your phone hotspot's name, do not include punctuation in name
const char* password = "YOUR_HOTSPOT_PASSWORD";  // your phone hotspot's password

// --- Where to send the OSC data (your laptop running Unreal) ---
IPAddress laptopIP(172, 20, 10, 2);   // your laptop's IP on the hotspot (CHANGE THIS)
const int laptopPort = 8000;          // the port Unreal listens on (must match Unreal)

WiFiUDP Udp;                          // the tool that actually sends the UDP packets

// --- Create the sensor object (55 is just an ID, 0x28 is its address) ---
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup() {
  Serial.begin(115200);              // start talking to the Serial Monitor

  // Start the I2C connection on the FireBeetle's default SDA/SCL pins
  Wire.begin();

  // Start the sensor; if it fails, stop here and warn
  if (!bno.begin()) {
    Serial.println("BNO055 not found! Check wiring.");
    while (1);                        // freeze here forever
  }

  // Connect to the phone hotspot
  Serial.print("Connecting to hotspot");
  WiFi.begin(ssid, password);        // start connecting
  while (WiFi.status() != WL_CONNECTED) {  // wait until connected
    delay(500);
    Serial.print(".");               // print dots while we wait
  }
  Serial.println(" connected!");

  // Show the ESP32's own IP address (useful for checking)
  Serial.print("My IP is: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read the sensor's orientation (yaw, pitch, roll) in degrees
  sensors_event_t orientationData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

  float yaw   = orientationData.orientation.x;  // heading / turning
  float pitch = orientationData.orientation.y;  // tilt forward/back
  float roll  = orientationData.orientation.z;  // tilt left/right

  // Read acceleration with gravity removed (linear acceleration)
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  float accZ = accel.z();            // up/down acceleration

  // --- Build one OSC message with all four values ---
  OSCMessage msg("/imu");            // "/imu" is the address Unreal will listen for
  msg.add(yaw);                      // add yaw as the 1st value
  msg.add(pitch);                    // add pitch as the 2nd value
  msg.add(roll);                     // add roll as the 3rd value
  msg.add(accZ);                     // add accZ as the 4th value

  // --- Send the message to the laptop ---
  Udp.beginPacket(laptopIP, laptopPort);  // start a packet aimed at the laptop
  msg.send(Udp);                          // put the OSC message inside it
  Udp.endPacket();                        // send it
  msg.empty();                            // clear the message for next time

  // Print to Serial Monitor so you can see it working
  Serial.print(yaw); Serial.print(", ");
  Serial.print(pitch); Serial.print(", ");
  Serial.print(roll); Serial.print(", ");
  Serial.println(accZ);

  delay(50);                          // send about 20 times per second
}