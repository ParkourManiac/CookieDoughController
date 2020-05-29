#include "Key.h"
#include <Arduino.h>

void ConfigurePinForKey(IKey &key)
{
    pinMode(key.pin, INPUT_PULLUP);
}

void DebounceRead(IPinState &key) // TODO: This causes a slight input delay. Consider this: if you were to press the button every <30ms the input would not be registered.
{
    key.oldValue = key.value;
    unsigned int debounceDelay = 30; // TODO: This balance needs to be play tested.
    unsigned long currentTime = millis();

    // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    bool pinState = !digitalRead(key.pin);

    // If the pin state has changed...
    if (pinState != key.oldPinState)
    {
        key.lastDebounceTime = currentTime;
        // // Print debounce catches.
        // Serial.print("he");
    }

    unsigned long timePassedSinceDebounce = (currentTime - key.lastDebounceTime);
    // If we've waited long enough since last debounce...
    if (timePassedSinceDebounce > debounceDelay)
    {
        // And if the old state is not already the new state...
        if (pinState != key.value)
        {
            key.value = pinState;

            if (key.value)
            {
                key.timeOfActivation = currentTime;
            }

            // // Print debounce catches.
            // if(key.value) {
            //     Serial.print(" hej");
            // } else {Serial.print(" hå");}
            // Serial.println();
        }
    }

    key.oldPinState = pinState;
}

bool OnKeyPress(IPinState &key)
{
    return (key.oldValue != key.value && key.value);
}

bool OnKeyRelease(IPinState &key)
{
    return (key.oldValue != key.value && !key.value);
}

bool OnLongPress(IPinState key, unsigned int longPressDuration)
{
    return (millis() - key.timeOfActivation) > longPressDuration;
}