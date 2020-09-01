#include "Key.h"
#include <Arduino.h>

void ConfigurePinForKey(const IKey &key)
{
    pinMode(key.pin, INPUT_PULLUP);
}

void DebounceRead(IPinState *key) // NOTE: This causes a slight input delay. Consider this: if you were to press the button every <30ms the input would not be registered.
{
    key->oldValue = key->value;
    unsigned int debounceDelay = 30; // TODO: This balance needs to be play tested.
    uint32_t currentTime = millis();

    bool pinState = digitalRead(key->pin);

    // If the pin state has changed...
    if (pinState != key->oldPinState)
    {
        key->lastDebounceTime = currentTime;
        // // Print debounce catches.
        // DEBUG_PRINT("he");
    }

    uint32_t timePassedSinceDebounce = (currentTime - key->lastDebounceTime);
    // If we've waited long enough since last debounce...
    if (timePassedSinceDebounce > debounceDelay)
    {
        // Invert key value to get pin state. Pullup is active low. 1 = off. 0 = on.
        bool pinStateOfKey = !key->value; 

        // If the state is outdated...
        if (pinState != pinStateOfKey)
        {
            // Invert input signal. Pullup is active low. 1 = off. 0 = on.
            key->value = !pinState;

            if (key->value)
            {
                key->timeOfActivation = currentTime;
            }

            // // Print debounce catches.
            // if(key->value) {
            //     DEBUG_PRINT(" hej");
            // } else {DEBUG_PRINT(" hå");}
            // DEBUG_PRINT("\n");
        }
    }

    key->oldPinState = pinState;
}

bool OnKeyPress(const IPinState &key)
{
    return (key.oldValue != key.value && key.value);
}

bool OnKeyRelease(const IPinState &key)
{
    return (key.oldValue != key.value && !key.value);
}

bool OnLongPress(const IPinState &key, unsigned int longPressDuration)
{
    return (millis() - key.timeOfActivation) >= longPressDuration;
}