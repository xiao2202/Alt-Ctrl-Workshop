/*
 * Alt.Ctrl Workshop — Session 1: HID Keyboard Emulation
 * Board: FireBeetle 2 ESP32-S3 (native USB HID)
 * FOUR copper-tape buttons → keyboard keys, for two-player Pong
 *
 * WIRING (each button = two copper pads that touch when pressed):
 *   Button 1: pad → IO13, other pad → GND   (Left  UP   = 'w')
 *   Button 2: pad → IO14, other pad → GND   (Left  DOWN = 's')
 *   Button 3: pad → IO12, other pad → GND   (Right UP   = up arrow)
 *   Button 4: pad → IO4,  other pad → GND   (Right DOWN = down arrow)
 *   No resistors needed — INPUT_PULLUP handles that internally.
 *   All GND pads can share one GND pin on the board.
 *
 * SETTINGS (Tools menu) — required or nothing works:
 *   USB Mode → "USB-OTG (TinyUSB)"   (turns the board into a keyboard)
 *   USB CDC On Boot → "Enabled"      (keeps Serial working over the same cable)
 *   Serial Monitor speed → 115200
 */

#include "USB.h"             // The ESP32's built-in USB engine (not from Library Manager!)
#include "USBHIDKeyboard.h"  // Lets the board pretend to be a keyboard

USBHIDKeyboard Keyboard;      // Create a fake keyboard named 'Keyboard'

// --- Describe each button in one place ---
// Add or change a button by editing a single line here.
const int NUM_BUTTONS = 4;

int  pins[NUM_BUTTONS] = { 13, 14, 12, 4 };   // the IO pin each button is wired to
int  keys[NUM_BUTTONS] = { 'w', 's', KEY_UP_ARROW, KEY_DOWN_ARROW };  // key each one presses
const char* names[NUM_BUTTONS] = { "Left UP", "Left DOWN", "Right UP", "Right DOWN" };

// Remember each button's state from last loop, so we act only on press/release.
bool prev[NUM_BUTTONS];

void setup() {
  Serial.begin(115200);     // Talk to the Serial Monitor (must match!)

  // Set every button pin as an input with pull-up, and mark it "not pressed".
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(pins[i], INPUT_PULLUP);
    prev[i] = HIGH;
  }

  Keyboard.begin();         // Wake up the fake keyboard
  USB.begin();              // Start the USB engine

  delay(1000);              // give the USB a second to connect
  Serial.println("HID ready. Bridge a pad to fire a key.");
}

void loop() {
  // Check all four buttons every loop.
  for (int i = 0; i < NUM_BUTTONS; i++) {
    bool now = digitalRead(pins[i]);   // LOW = pads bridged = pressed

    if (now == LOW && prev[i] == HIGH) {   // just pressed
      Keyboard.press(keys[i]);             // hold the key DOWN
      Serial.print(names[i]);
      Serial.println(" pressed");
    }
    if (now == HIGH && prev[i] == LOW) {   // just released
      Keyboard.release(keys[i]);           // let the key UP
      Serial.print(names[i]);
      Serial.println(" released");
    }

    prev[i] = now;   // remember for next loop
  }

  delay(10);         // simple debounce
}