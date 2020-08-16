#include <Arduino.h>

#include <Controller.h>
#include <SRAM.cpp>  // DEBUG

// #define DEBUG_PRINT(VALUE) Serial.print(VALUE)
// #define DEBUG_PRINTLN(VALUE) Serial.println(VALUE)

/**
 * @brief A pre-defined controller object, set up with the prefered keymap settings.
 * 
 * @note To configure the controller and run the program:
 * 1. Create an external config file ending with the extension ".cpp" in the src folder.
 * 2. Define a variable of type "Controller" named "controller".
 * 3. Call the constructor of the "Controller" object with your prefered default and special function keymaps.
 * This will configure the controller to use your own custom configuration.
 */
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
    // Without this delay the serial port sends a few weird null letters in serial monitor when initially connected.
    delay(1000);

    controller.Setup(); // SRAM: -??
}

void loop()
{
    controller.Update(); // SRAM: -??
    delay(1000); // DEBUG
    Serial.println(freeMemory()); // DEBUG
}

// "If you run out of SRAM..." - https://www.arduino.cc/en/tutorial/memory

// TODO: ADD ISSUE ON GITHUB. "Förhindra att mikrokontrollern får slut på SRAM, eller att man skriver in för många keymaps i EEPROM."