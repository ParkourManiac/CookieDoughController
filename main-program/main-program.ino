const int inputPin = 2;

uint8_t buf[8] = { 
  0 };   /* Keyboard report buffer */
bool oldResult = false;

void setup() {
  pinMode(inputPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  bool result = !digitalRead(inputPin); // Invert input signal. Pullup is active low. 1 = off. 0 = on.

  if(oldResult != result) 
  {
    if(result) 
    {
      Serial.println(result);
      
      // Send keypress
      buf[2] = 26;
      Serial.write(buf, 8);   
    } 
    else 
    {
      Serial.println(result);
      
       // Send release keypress
      buf[0] = 0;
      buf[2] = 0;
      Serial.write(buf, 8); 
    }  
  }
  
  oldResult = result;
  digitalWrite(LED_BUILTIN, result);
}
