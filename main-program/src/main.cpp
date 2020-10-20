#include <Arduino.h>

#include <Controller.h>
#include <MeasureSRAM.h>

// #define DEBUG(VALUE) VALUE
// #define DEBUG_PRINT(VALUE) Serial.print(VALUE)

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

    controller.Setup();
}

void loop()
{
    controller.Update();

    // DEBUG
    if(millis() % 1000 == 0) {
        Serial.print(freeMemory());
        Serial.print(":");
        Serial.print(controller.CalculateUnusedStorage());
        Serial.print("\n");
    } 
    // DEBUG
}

// TODO: Learn how to use CMake and try using it in the project.
// TODO: Learn how to use clang-tidy and use it in the project. https://llvm.org/
// TODO: When in doubt: Use assert().
// TODO: __attribute__((packed)) on the structs that need to be saved to eeprom. If neccessary
// TODO: Research POD type. Plain-Old-datatype. (Serialization for c++).
// TODO: Look into using GDB for debugging. b <num> = sets breakpoint. r = run code. c = Continue to next breakpoint. p <expression> = Prints the result of the given expression.
