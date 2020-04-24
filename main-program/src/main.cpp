#include <Arduino.h>

// Struct
struct Key {
    int pin;
    int keyCode;
    bool value;
    bool oldValue;
};

// HEADER
void ReadPinValueForKeys();
void SendKeyInfo();

// Public values
const int inputPin = 2;
const int inputPinKeycode = 26;

Key keys[3] = { 
    {.pin = 2, .keyCode = 26},
    // {.pin = 2, .keyCode = 27},
    // {.pin = 2, .keyCode = 28},
};

uint8_t buf[8] = {0}; // Keyboard report buffer.

// PROGRAM
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    // Set keys as Inputs with internal pullups.
    for(Key key : keys) {
        pinMode(key.pin, INPUT_PULLUP);
    }
}

void loop()
{
    ReadPinValueForKeys();

    SendKeyInfo();
}

void ReadPinValueForKeys() // TODO: Test if working
{
    for(Key key : keys) {
        key.value = !digitalRead(inputPin); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    }
}

void SendKeyInfo() { // TODO: Handle debounce. Get values to update.
    for(Key key : keys) {
        if (key.oldValue != key.value)
        {
            digitalWrite(LED_BUILTIN, HIGH);
            //Serial.println(key.value);
            if (key.value)
            {
                digitalWrite(LED_BUILTIN, HIGH);

                // Send keypress
                buf[2] = key.keyCode;
                Serial.write(buf, 8);
            }
            else
            {
                digitalWrite(LED_BUILTIN, LOW);

                // Send release keypress
                buf[0] = 0;
                buf[2] = 0;
                Serial.write(buf, 8);
            }
        }

        key.oldValue = key.value;
    }
}

