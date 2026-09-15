/*
 * Alt.Ctrl Workshop — Session 1: HID Keyboard Emulation
 * Board: Arduino Nano 33 IoT (Native USB HID)
 * FOUR copper-tape buttons → keyboard keys, for two-player Pong
 *
 * WIRING (each button = two copper pads that touch when pressed):
 *   Button 1: one end → D2, other end → GND   (Left  UP   = 'w')
 *   Button 2: one end → D3, other end → GND   (Left  DOWN = 's')
 *   Button 3: one end → D4, other end → GND   (Right UP   = up arrow)
 *   Button 4: one end → D5, other end → GND   (Right DOWN = down arrow)
 *   No resistors needed — INPUT_PULLUP handles that internally.
 *   All GND pads can share any GND pin on the Nano 33 IoT.
 *
 * IDE SETTINGS:
 *   Board: Arduino NANO 33 IoT
 *   Serial Monitor speed: 115200
 */

#include <Keyboard.h> // Official Arduino USB HID library for SAMD21

const int NUM_BUTTONS = 4;

// Use standard digital pins on the Nano 33 IoT
int  pins[NUM_BUTTONS] = { 2, 3, 4, 5 }; 
int  keys[NUM_BUTTONS] = { 'w', 's', KEY_UP_ARROW, KEY_DOWN_ARROW }; 
const char* names[NUM_BUTTONS] = { "Left UP", "Left DOWN", "Right UP", "Right DOWN" }; 

bool prev[NUM_BUTTONS];

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(pins[i], INPUT_PULLUP);
    prev[i] = HIGH;
  }

  Keyboard.begin(); // Start USB Keyboard emulation
  delay(1000);     // Allow USB connection time to settle
  Serial.println("HID ready. Bridge a pad to fire a key.");
}

void loop() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    bool now = digitalRead(pins[i]);

    if (now == LOW && prev[i] == HIGH) {   // Just pressed
      Keyboard.press(keys[i]);
      Serial.print(names[i]);
      Serial.println(" pressed");
    }
    if (now == HIGH && prev[i] == LOW) {   // Just released
      Keyboard.release(keys[i]);
      Serial.print(names[i]);
      Serial.println(" released");
    }

    prev[i] = now;
  }

  delay(10); // Simple debounce
}