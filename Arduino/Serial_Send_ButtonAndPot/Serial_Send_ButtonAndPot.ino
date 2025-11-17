int potPin = A0;
int buttonPin = 2;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
}

void loop() {
  int potVal = analogRead(potPin);
  int buttonVal = digitalRead(buttonPin);
  Serial.print(potVal);
  Serial.print(",");
  Serial.println(buttonVal);

  delay(10);
}
