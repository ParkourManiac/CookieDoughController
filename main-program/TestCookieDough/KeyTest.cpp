#include "test.h"
#include "../lib/CookieDoughLibrary/Key.h"
#include "Fakes/Arduino.h"

extern uint8_t pinMode_param_pin;
extern uint8_t pinMode_param_mode;
extern unsigned int pinMode_invocations;

extern unsigned long millis_return;

extern int digitalRead_return;
extern uint8_t digitalRead_param_pin;
extern unsigned int digitalRead_invocations;

void ConfigurePinForKey_IKeysPinIsPassedToPinMode()
{
    int expectedPin = 2;
    IKey key = IKey{.pin = expectedPin};

    ConfigurePinForKey(key);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinForKeyOfTypeKey_IsCorrectlyParsedToIKey()
{
    int expectedPin = 2;
    Key key = Key(expectedPin, 1337);

    ConfigurePinForKey(key);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinForKeyOfTypeSpecialKey_IsCorrectlyParsedToIKey()
{
    int expectedPin = 2;
    SpecialKey key = SpecialKey(expectedPin, toggleDefaultKeyMap);

    ConfigurePinForKey(key);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinForKey_ConfiguresPinAsInputPullup()
{
    IKey key = IKey{.pin = 2};

    ConfigurePinForKey(key);

    ASSERT_TEST(pinMode_param_mode == INPUT_PULLUP);
}

void ConfigurePinForKey_CallsPinModeOnce()
{
    SpecialKey key = SpecialKey(2, toggleDefaultKeyMap);

    ConfigurePinForKey(key);

    ASSERT_TEST(1 == pinMode_invocations);
}

void OnKeyPressPinStateWentFromInactiveToActive_ReturnsTrue()
{
    IPinState state;
    state.oldValue = false;
    state.value = true;

    bool result = OnKeyPress(state);

    ASSERT_TEST(result == true);
}

void OnKeyPressPinStateValueDidNotChange_ReturnsFalse()
{
    IPinState state;
    state.oldValue = true;
    state.value = true;

    bool result = OnKeyPress(state);

    ASSERT_TEST(result == false);
}

void OnKeyPressPinStateIsInactive_ReturnFalse()
{
    IPinState state;
    state.value = false;

    bool result = OnKeyPress(state);

    ASSERT_TEST(result == false);
}

void OnKeyReleasePinStateChangedToInactive_ReturnTrue()
{
    IPinState state;
    state.oldValue = true;
    state.value = false;

    bool result = OnKeyRelease(state);

    ASSERT_TEST(result == true);
}

void OnKeyReleasePinStateDidNotChange_ReturnFalse()
{
    IPinState state;
    state.oldValue = false;
    state.value = false;

    bool result = OnKeyRelease(state);

    ASSERT_TEST(result == false);
}

void OnKeyReleasePinStateIsActive_ReturnFalse()
{
    IPinState state;
    state.value = true;

    bool result = OnKeyRelease(state);

    ASSERT_TEST(result == false);
}

void OnLongPressTimePassedIsNotGreaterThanLongPressDuration_ReturnFalse()
{
    IPinState state;
    int currentTime = 0;
    state.timeOfActivation = 0;
    int longPressDuration = 5;

    millis_return = currentTime;
    bool result = OnLongPress(state, longPressDuration);

    ASSERT_TEST(result == false);
}

void OnLongPressTimePassedIsGreaterThanLongPressDuration_ReturnTrue()
{
    IPinState state;
    int currentTime = 10;
    state.timeOfActivation = 0;
    int longPressDuration = 5;

    millis_return = currentTime;
    bool result = OnLongPress(state, longPressDuration);

    ASSERT_TEST(result == true);
}

void OnLongPressTimePassedIsEqualToLongPressDuration_ReturnTrue()
{
    IPinState state;
    int currentTime = 5;
    state.timeOfActivation = 0;
    int longPressDuration = currentTime - state.timeOfActivation;

    millis_return = currentTime;
    bool result = OnLongPress(state, longPressDuration);

    ASSERT_TEST(result == true);
}

void ConfigurePinsForKeyMap_InvokesPinModeForEachKey()
{
    int length = 3;
    Key keymap[length] = {
        Key(2, 1337),
        Key(3, 1337),
        Key(4, 1337),
    };

    ConfigurePinsForKeyMap(keymap, length);

    ASSERT_TEST(pinMode_invocations == 3);
}

void ConfigurePinsForKeyMap_CallsPinModeWithPinFromKeyMap()
{
    int length = 1;
    Key keymap[length] = {
        Key(13, 1337),
    };
    int correctPin = 13;

    ConfigurePinsForKeyMap(keymap, length);

    ASSERT_TEST(pinMode_param_pin == correctPin);
}

void ConfigurePinsForKeyMap_SetsUpPinAsInputPullup()
{
    int length = 1;
    Key keymap[length] = {
        Key(13, 1337),
    };

    ConfigurePinsForKeyMap(keymap, length);

    ASSERT_TEST(pinMode_param_mode == INPUT_PULLUP);
}

void DebounceReadPinState_UpdatesPinStateOldValueWithPreviousPinStateValue()
{
    IPinState state;
    state.oldValue = false;
    state.value = true;

    DebounceRead(state);

    ASSERT_TEST(state.oldValue == true);
}

void DebounceReadPinState_ReadsStateOfPin()
{
    IPinState state;
    state.pin = 3;

    DebounceRead(state);

    ASSERT_TEST(digitalRead_param_pin == 3 && digitalRead_invocations == 1);
}

// void DebounceReadPinStateStateHasChanged_UpdatesLastDebounceTime() {
//     IPinState state;
//     state.oldPinState = false;
//     state.

// }

// ReadPinValuesForKeyMap