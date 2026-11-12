/*
 * ============================================================
 *  Alt.Ctrl — One Button, Everything Reacts (Unity -> ESP32)
 *  Board: FireBeetle 2 ESP32-S3
 *
 *  Unity sends 'P' when the player clicks the 3D button.
 *  The board then, ALL AT THE SAME TIME, for a short burst:
 *     - flashes the LED
 *     - beeps the buzzer on and off
 *     - sweeps the servo back and forth
 *  After the burst ends, everything stops and waits for the
 *  next press.
 *
 *  This uses millis() instead of delay(), so all three run
 *  together (delay would make them happen one after another).
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

// --- Burst control ---
bool bursting = false;             // are we currently doing the burst?
unsigned long burstStart = 0;      // when the burst began (in milliseconds)
const unsigned long BURST_LENGTH = 2000;  // how long the whole burst lasts (ms)

// --- Timers for each output (so each can run at its own speed) ---
unsigned long lastLedTime = 0;     // last time the LED changed
unsigned long lastServoTime = 0;   // last time the servo stepped
bool ledState = false;             // is the LED currently on?
int servoAngle = 0;                // current servo angle
int servoDir = 1;                  // which way the servo is moving (+1 or -1)

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  Serial.println("Ready. Click the button in Unity.");
}

void loop() {
  unsigned long now = millis();   // the current time, in milliseconds

  // --- Did Unity send a press? ---
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 'P' && !bursting) {   // start a burst (ignore if already bursting)
      bursting = true;
      burstStart = now;
      Serial.println("Burst start!");
    }
  }

  // --- If we're in a burst, run all three outputs together ---
  if (bursting) {

    // Stop the burst once enough time has passed
    if (now - burstStart >= BURST_LENGTH) {
      bursting = false;
      digitalWrite(LED_PIN, LOW);   // make sure LED ends off
      noTone(BUZZER_PIN);           // make sure buzzer ends silent
      myServo.write(0);             // return servo to start
      Serial.println("Burst done.");
      return;                        // skip the rest this loop
    }

    // ----- LED: flash on/off every 100 ms -----
    if (now - lastLedTime >= 100) {
      lastLedTime = now;
      ledState = !ledState;                       // flip it
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }

    // ----- Buzzer: beep follows the LED (on when LED is on) -----
    if (ledState) {
      tone(BUZZER_PIN, 1000);   // start a 1000 Hz tone (no duration = keep playing)
    } else {
      noTone(BUZZER_PIN);       // silence
    }

    // ----- Servo: step one degree every 5 ms, bouncing 0<->180 -----
    if (now - lastServoTime >= 5) {
      lastServoTime = now;
      servoAngle += servoDir;              // move one step
      if (servoAngle >= 180) servoDir = -1; // hit the top, reverse
      if (servoAngle <= 0)   servoDir = 1;  // hit the bottom, reverse
      myServo.write(servoAngle);
    }
  }
}