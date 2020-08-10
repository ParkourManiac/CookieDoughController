#include <Arduino.h>

#include <Controller.h>

// TODO: Copy this description into the readme file of the project.
/**
 * @brief A pre-defined controller object, set up with the prefered keymap settings.
 * 
 * @note To configure the controller and run the program:
 * 1. Create an external config file ending with the extension ".cpp" in the src folder.
 * 2. Define a variable of type "Controller" called "controller".
 * 3. Call the constructor of the "Controller" object with your prefered default and special function keymap.
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

    controller.Setup();
}

void loop()
{
    controller.Update();
}

