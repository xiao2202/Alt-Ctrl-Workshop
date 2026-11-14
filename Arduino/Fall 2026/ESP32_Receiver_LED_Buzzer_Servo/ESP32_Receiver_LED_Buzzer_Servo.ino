/*
 * ============================================================
 *  Alt.Ctrl — Unreal triggers the physical world over OSC
 *  Board: FireBeetle 2 ESP32-S3   (NON-BLOCKING version)
 *  Network: phone hotspot (same one Unreal/laptop is on)
 *
 *  Unreal SENDS OSC during a cutscene; this board RECEIVES and reacts:
 *     /led     -> flash the LED a few times
 *     /buzzer  -> beep the buzzer
 *     /servo   -> sweep the servo once
 *
 *  Uses millis() (not delay) so the board keeps listening for new
 *  OSC messages even while an output is still playing. Cutscene
 *  events can fire close together without being missed.
 *
 *  WIRING:
 *     LED:    IO4 -> 220 ohm -> LED -> GND
 *     Buzzer: IO5 -> buzzer -> GND
 *     Servo:  signal -> IO6, power -> VBUS (5V), ground -> GND
 *
 *  SETUP: read this board's IP from the Serial Monitor and type
 *         it into Unreal's OSC sender. Both on the same hotspot.
 *
 *  LIBRARIES (Library Manager): ESP32Servo, OSC (by CNMAT)
 * ============================================================
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <ESP32Servo.h>

// --- Your phone hotspot (CHANGE THESE) ---
const char* ssid     = "xiaowifi";   // no punctuation in the name
const char* password = "12345678";

// --- The port this board LISTENS on (must match Unreal) ---
const int listenPort = 8000;

// --- Pins ---
const int LED_PIN    = 4;
const int BUZZER_PIN = 5;
const int SERVO_PIN  = 6;

WiFiUDP Udp;
Servo myServo;

// ---------------- LED flashing state ----------------
bool ledActive = false;
unsigned long ledStart = 0;
unsigned long lastLedToggle = 0;
bool ledState = false;
const unsigned long LED_DURATION = 600;   // total flash time (ms)
const unsigned long LED_INTERVAL = 100;   // flip every 100 ms

// ---------------- Buzzer state ----------------
bool buzzerActive = false;
unsigned long buzzerStart = 0;
const unsigned long BUZZER_DURATION = 300; // beep length (ms)

// ---------------- Servo sweep state ----------------
bool servoActive = false;
unsigned long lastServoStep = 0;
int servoAngle = 0;
int servoDir = 1;
const unsigned long SERVO_INTERVAL = 5;    // step every 5 ms

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(0);

  // Connect to the hotspot (gives up after ~10s, won't freeze)
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
    Serial.println("Connected!");
    Serial.print("THIS BOARD'S IP (put this in Unreal): ");
    Serial.println(WiFi.localIP());
    Udp.begin(listenPort);
    Serial.print("Listening for OSC on port ");
    Serial.println(listenPort);
  } else {
    Serial.println("Could not connect. Check hotspot name/password (2.4GHz).");
  }
}

// --- OSC handlers: they START an output, they do NOT block ---

void onLed(OSCMessage &msg) {
  Serial.println("LED triggered");
  ledActive = true;
  ledStart = millis();
  lastLedToggle = millis();
  ledState = true;
  digitalWrite(LED_PIN, HIGH);
}

void onBuzzer(OSCMessage &msg) {
  Serial.println("Buzzer triggered");
  buzzerActive = true;
  buzzerStart = millis();
  tone(BUZZER_PIN, 1000);   // start tone; we stop it ourselves later
}

void onServo(OSCMessage &msg) {
  Serial.println("Servo triggered");
  servoActive = true;
  servoAngle = 0;
  servoDir = 1;
  lastServoStep = millis();
}

void loop() {
  unsigned long now = millis();

  // ---------- 1. Always listen for new OSC messages ----------
  OSCMessage msg;
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/led",    onLed);
      msg.dispatch("/buzzer", onBuzzer);
      msg.dispatch("/servo",  onServo);
    }
  }

  // ---------- 2. LED flashing ----------
  if (ledActive) {
    if (now - lastLedToggle >= LED_INTERVAL) {
      lastLedToggle = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
    if (now - ledStart >= LED_DURATION) {
      ledActive = false;
      digitalWrite(LED_PIN, LOW);
    }
  }

  // ---------- 3. Buzzer ----------
  if (buzzerActive) {
    if (now - buzzerStart >= BUZZER_DURATION) {
      buzzerActive = false;
      noTone(BUZZER_PIN);
    }
  }

  // ---------- 4. Servo sweep ----------
  if (servoActive) {
    if (now - lastServoStep >= SERVO_INTERVAL) {
      lastServoStep = now;
      servoAngle += servoDir;
      myServo.write(servoAngle);
      if (servoAngle >= 180) servoDir = -1;
      if (servoAngle <= 0 && servoDir == -1) {
        servoActive = false;
        myServo.write(0);
      }
    }
  }
}