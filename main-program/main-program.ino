const int inputPin = 2;

void setup() {
  pinMode(inputPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  bool result = !digitalRead(inputPin); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
  
  digitalWrite(LED_BUILTIN, result);
  Serial.println(result);
}
