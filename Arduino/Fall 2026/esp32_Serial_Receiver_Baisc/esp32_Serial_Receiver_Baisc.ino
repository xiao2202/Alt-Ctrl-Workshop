/*
 * ============================================================
 *  Alt.Ctrl — The Game Controls the Real World
 *  Board: FireBeetle 2 ESP32-S3
 *
 *  Unity sends a character over the USB cable, and the board
 *  makes something happen in the real world:
 *     press '1' -> turn the LED on or off
 *     press '2' -> make the buzzer beep
 *     press '3' -> sweep the servo (a little wave)
 *
 *  Build it up ONE piece at a time. The code already handles
 *  all three, so you only add HARDWARE, never change the code:
 *     Step 1: wire the LED      -> test key 1
 *     Step 2: add the buzzer    -> test key 2
 *     Step 3: add the servo     -> test key 3
 *  A part that isn't wired yet simply does nothing. It won't
 *  break the parts that are already working.
 *
 *  WIRING:
 *     LED:    IO4 -> 220 ohm resistor -> LED -> GND
 *     Buzzer: IO5 -> buzzer -> GND
 *     Servo:  signal -> IO6,  power -> VCC (5V),  ground -> GND
 *     (All grounds share GND. Servo power uses VCC, not 3V3.)
 *
 *  LIBRARY to install (Library Manager): ESP32Servo
 * ============================================================
 */

#include <ESP32Servo.h>   // lets us control a servo motor

// --- Which pin each part is connected to ---
const int LED_PIN    = 4;   // the LED is on IO4
const int BUZZER_PIN = 5;   // the buzzer is on IO5
const int SERVO_PIN  = 6;   // the servo's signal wire is on IO6

// --- Create a servo object we can control ---
Servo myServo;

// --- Remember whether the LED is currently on or off ---
bool ledOn = false;   // starts off


void setup() {
  // setup() runs once when the board starts

  Serial.begin(115200);        // start talking to the computer (must match Unity)

  pinMode(LED_PIN, OUTPUT);    // tell the board the LED pin is an OUTPUT
  pinMode(BUZZER_PIN, OUTPUT); // tell the board the buzzer pin is an OUTPUT

  myServo.attach(SERVO_PIN);   // connect our servo object to its pin
  myServo.write(0);            // move the servo to its starting position (0 degrees)

  Serial.println("Ready. Send 1 (LED), 2 (buzzer), or 3 (servo).");
}


void loop() {
  // loop() runs over and over, forever

  // Is there a new character waiting from Unity?
  if (Serial.available() > 0) {

    char c = Serial.read();    // read ONE character that Unity sent

    // ---------- Key 1: the LED ----------
    if (c == '1') {
      ledOn = !ledOn;                            // flip on->off or off->on
      digitalWrite(LED_PIN, ledOn ? HIGH : LOW); // HIGH = on, LOW = off
      Serial.println("LED toggled");             // print so we can see it worked
    }

    // ---------- Key 2: the buzzer ----------
    else if (c == '2') {
      tone(BUZZER_PIN, 1000, 200);   // play a 1000 Hz sound for 200 milliseconds
      Serial.println("Buzzer beep");
    }

    // ---------- Key 3: the servo sweep ----------
    else if (c == '3') {
      Serial.println("Servo sweep");

      // Move slowly from 0 up to 180 degrees
      for (int angle = 0; angle <= 180; angle++) {
        myServo.write(angle);   // move to this angle
        delay(5);               // tiny pause so the motion is smooth, not instant
      }

      // Then move slowly back from 180 to 0 degrees
      for (int angle = 180; angle >= 0; angle--) {
        myServo.write(angle);
        delay(5);
      }
    }
  }
}