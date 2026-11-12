/*
 * ============================================================
 *  Alt.Ctrl — One Button, Everything Reacts (Unity -> ESP32)
 *  Board: FireBeetle 2 ESP32-S3
 *
 *  Unity sends 'P' when the player clicks the 3D button.
 *  The board runs all three at once for a short burst:
 *     - flashes the LED
 *     - beeps the buzzer
 *     - sweeps the servo back and forth
 *
 *  Uses millis() so all three run together.
 *
 *  WIRING:
 *     LED:    IO4 -> 220 ohm resistor -> LED -> GND
 *     Buzzer: IO5 -> buzzer -> GND
 *     Servo:  signal -> IO6,  power -> VBUS (5V),  ground -> GND
 *
 *  LIBRARY (Library Manager): ESP32Servo
 * ============================================================
 */

#include <ESP32Servo.h>

const int LED_PIN    = 4;
const int BUZZER_PIN = 5;
const int SERVO_PIN  = 6;

Servo myServo;

bool bursting = false;
unsigned long burstStart = 0;
const unsigned long BURST_LENGTH = 2000;

unsigned long lastLedTime = 0;
unsigned long lastServoTime = 0;
bool ledState = false;
int servoAngle = 0;
int servoDir = 1;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  Serial.println("Ready. Click the button in Unity.");
}

void loop() {
  unsigned long now = millis();

  // --- Did Unity send a press? ---
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 'P' && !bursting) {
      bursting = true;
      burstStart = now;
      Serial.println("Burst start!");
    }
  }

  // --- Run all three together during the burst ---
  if (bursting) {

    // End the burst after BURST_LENGTH
    if (now - burstStart >= BURST_LENGTH) {
      bursting = false;
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      myServo.write(0);
      Serial.println("Burst done.");
      return;
    }

    // LED flashes every 100 ms, and the buzzer follows it
    if (now - lastLedTime >= 100) {
      lastLedTime = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);

      // Buzzer beeps each time the LED turns on
      if (ledState) {
        tone(BUZZER_PIN, 1000, 80);   // short 80 ms beep, same as your working code
      }
    }

    // Servo steps every 5 ms, bouncing 0<->180
    if (now - lastServoTime >= 5) {
      lastServoTime = now;
      servoAngle += servoDir;
      if (servoAngle >= 180) servoDir = -1;
      if (servoAngle <= 0)   servoDir = 1;
      myServo.write(servoAngle);
    }
  }
}