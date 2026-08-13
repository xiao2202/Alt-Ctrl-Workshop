int buttonPin = 2;
int potPin = A0;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
}

void loop() {
  int buttonVal = digitalRead(buttonPin);
  int potVal = analogRead(potPin);

  Serial.print(buttonVal);
  Serial.print(",");
  Serial.println(potVal);

  delay(10);
}
