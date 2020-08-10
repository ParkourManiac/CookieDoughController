#include <Arduino.h>

#include <Controller.h>

extern Controller controller;

// PROGRAM
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    while (!Serial)
    {
        // Wait for serial port to start.
    }

    controller.Setup();
}

void loop()
{
    controller.Update();
}

