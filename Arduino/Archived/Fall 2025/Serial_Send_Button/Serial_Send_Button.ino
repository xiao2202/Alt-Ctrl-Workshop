int buttonPin = 2;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
}

void loop() {
  int buttonVal = digitalRead(buttonPin);
  Serial.println(buttonVal);

  delay(10);
}
