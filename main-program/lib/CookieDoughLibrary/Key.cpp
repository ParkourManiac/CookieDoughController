#include "Key.h"
#include <Arduino.h>

void ConfigurePinForKey(const IKey &pin)
{
    pinMode(pin, INPUT_PULLUP);
}

void DebounceRead(IKey pin, IPinState *state) // NOTE: This causes a slight input delay. Consider this: if you were to press the button every <30ms the input would not be registered.
{
    state->oldValue = state->value;
    unsigned int debounceDelay = 30; // TODO: This balance needs to be play tested.
    uint32_t currentTime = millis();

    bool pinState = digitalRead(pin);

    // If the pin state has changed...
    if (pinState != state->oldPinState)
    {
        state->lastDebounceTime = currentTime;
        // // Print debounce catches.
        // DEBUG_PRINT("he");
    }

    uint32_t timePassedSinceDebounce = (currentTime - state->lastDebounceTime);
    // If we've waited long enough since last debounce...
    if (timePassedSinceDebounce > debounceDelay)
    {
        // Invert key value to get pin state. Pullup is active low. 1 = off. 0 = on.
        bool pinStateOfKey = !state->value; 

        // If the state is outdated...
        if (pinState != pinStateOfKey)
        {
            // Invert input signal. Pullup is active low. 1 = off. 0 = on.
            state->value = !pinState;

            if (state->value)
            {
                state->timeOfActivation = currentTime;
            }

            // // Print debounce catches.
            // if(state->value) {
            //     DEBUG_PRINT(" hej");
            // } else {DEBUG_PRINT(" hå");}
            // DEBUG_PRINT("\n");
        }
    }

    state->oldPinState = pinState;
}

bool OnKeyPress(const IPinState &state)
{
    return (state.oldValue != state.value && state.value);
}

bool OnKeyRelease(const IPinState &state)
{
    return (state.oldValue != state.value && !state.value);
}

bool OnLongPress(const IPinState &state, unsigned int longPressDuration)
{
    return (millis() - state.timeOfActivation) >= longPressDuration;
}