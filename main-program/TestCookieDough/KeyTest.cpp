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

void OnKeyPress_PinStateWentFromInactiveToActive_ReturnsTrue()
{
    IPinState state;
    state.oldValue = false;
    state.value = true;

    bool result = OnKeyPress(state);

    ASSERT_TEST(result == true);
}

void OnKeyPress_PinStateValueDidNotChange_ReturnsFalse()
{
    IPinState state;
    state.oldValue = true;
    state.value = true;

    bool result = OnKeyPress(state);

    ASSERT_TEST(result == false);
}

void OnKeyPress_PinStateIsInactive_ReturnFalse()
{
    IPinState state;
    state.value = false;

    bool result = OnKeyPress(state);

    ASSERT_TEST(result == false);
}

void OnKeyRelease_PinStateChangedToInactive_ReturnTrue()
{
    IPinState state;
    state.oldValue = true;
    state.value = false;

    bool result = OnKeyRelease(state);

    ASSERT_TEST(result == true);
}

void OnKeyRelease_PinStateDidNotChange_ReturnFalse()
{
    IPinState state;
    state.oldValue = false;
    state.value = false;

    bool result = OnKeyRelease(state);

    ASSERT_TEST(result == false);
}

void OnKeyRelease_PinStateIsActive_ReturnFalse()
{
    IPinState state;
    state.value = true;

    bool result = OnKeyRelease(state);

    ASSERT_TEST(result == false);
}

void OnLongPress_TimePassedIsNotGreaterThanLongPressDuration_ReturnFalse()
{
    IPinState state;
    int currentTime = 0;
    state.timeOfActivation = 0;
    int longPressDuration = 5;

    millis_return = currentTime;
    bool result = OnLongPress(state, longPressDuration);

    ASSERT_TEST(result == false);
}

void OnLongPress_TimePassedIsGreaterThanLongPressDuration_ReturnTrue()
{
    IPinState state;
    int currentTime = 10;
    state.timeOfActivation = 0;
    int longPressDuration = 5;

    millis_return = currentTime;
    bool result = OnLongPress(state, longPressDuration);

    ASSERT_TEST(result == true);
}

void OnLongPress_TimePassedIsEqualToLongPressDuration_ReturnTrue()
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

void DebounceRead_UpdatesOldValueOfStateWithPreviousStateValue()
{
    IPinState state;
    state.oldValue = false;
    state.value = true;

    DebounceRead(state);

    ASSERT_TEST(state.oldValue == true);
}

void DebounceRead_ReadsStateOfPin()
{
    IPinState state;
    state.pin = 3;

    DebounceRead(state);

    ASSERT_TEST(digitalRead_param_pin == 3 && digitalRead_invocations == 1);
}

void DebounceRead_PinStateHasChanged_UpdatesLastDebounceTime()
{
    IPinState state;
    unsigned long expectedDebounceTime = 1337;
    millis_return = expectedDebounceTime;
    state.lastDebounceTime = 0;
    state.oldPinState = true;
    digitalRead_return = false;

    DebounceRead(state);

    ASSERT_TEST(state.lastDebounceTime == expectedDebounceTime);
}

void DebounceRead_PinStateHasNotChanged_DoesNotUpdateLastDebounceTime()
{
    IPinState state;
    unsigned long expectedDebounceTime = 0;
    state.lastDebounceTime = expectedDebounceTime;
    millis_return = 1337;
    state.oldPinState = true;
    digitalRead_return = true;

    DebounceRead(state);

    ASSERT_TEST(state.lastDebounceTime == expectedDebounceTime);
}

void DebounceRead_DebounceTimeExceededAndValueIsOutdated_UpdateStateValue()
{
    IPinState state;
    state.lastDebounceTime = 0;
    millis_return = state.lastDebounceTime + 100000;
    digitalRead_return = true; // true = Button is released.
    state.oldPinState = true;
    state.value = true; // Button was previously pressed.
    bool expected = false; // We expect the button to be released.

    DebounceRead(state);

    ASSERT_TEST(state.value == expected);
}

void DebounceRead_DebounceTimeExceededAndValueIsOutdatedAndTheNewValueIsActive_UpdateTheTimeOfActivation()
{
    IPinState state;
    state.timeOfActivation = 0;
    unsigned long expectedTimeOfActivation = state.lastDebounceTime + 100000;
    millis_return = expectedTimeOfActivation;
    bool currentPinState = false; // false = Button is pressed.
    digitalRead_return = currentPinState;
    state.oldPinState = currentPinState;
    state.value = false; // Button was previously released.

    DebounceRead(state);

    ASSERT_TEST(state.timeOfActivation == expectedTimeOfActivation);
}

void DebounceRead_ValueIsOutdatedButDebounceTimeIsNotExceeded_DoesNotUpdateStateValue()
{
    digitalRead_return = true; // true = Button is released.
    IPinState state;
    state.lastDebounceTime = 0;
    millis_return = state.lastDebounceTime;
    bool expected = true;   // We expect the button to still be pressed.
    state.value = expected; // Button was previously pressed.

    DebounceRead(state);

    ASSERT_TEST(state.value == expected);
}

void DebounceRead_DebounceTimeExceededAndTheValueIsActiveButValueIsNotOutdated_DoesNotUpdateTheTimeOfActivation()
{
    bool currentPinState = false; // false = Button is pressed.
    digitalRead_return = currentPinState;
    IPinState state;
    state.oldPinState = !currentPinState;
    state.value = true;                   // Button was previously released.
    millis_return = state.lastDebounceTime + 100000;
    unsigned long expectedTimeOfActivation = 1337;
    state.timeOfActivation = expectedTimeOfActivation;

    DebounceRead(state);

    ASSERT_TEST(state.timeOfActivation == expectedTimeOfActivation);
}

void DebounceRead_OldPinStateIsUpdated()
{
    IPinState state;
    digitalRead_return = false;        // Button is pressed.
    state.oldPinState = true;         // Button was released.
    bool expectedOldPinState = false; // We expect it to become released.

    DebounceRead(state);

    ASSERT_TEST(state.oldPinState == expectedOldPinState);
}

void ReadPinValuesForKeyMap_CallsDigitalReadForEachItem()
{
    int length = 2;
    Key keymap[length] = {
        Key(2, 1337),
        Key(3, 1337),
    };

    ReadPinValuesForKeyMap(keymap, length);

    ASSERT_TEST(digitalRead_invocations == length);
}

void ReadPinValuesForKeyMap_CorrectlyParsesKeyPin()
{
    int length = 1;
    Key keymap[length] = {
        Key(3, 1337),
    };
    int expectedPin = 3;

    ReadPinValuesForKeyMap(keymap, length);

    ASSERT_TEST(digitalRead_param_pin == expectedPin);
}

void ReadPinValuesForKeyMap_UpdatesStateForAllPins()
{
    digitalRead_return = true;
    int length = 2;
    Key keymap[length] = {
        Key(2, 1337),
        Key(3, 1337),
    };

    ReadPinValuesForKeyMap(keymap, length);

    ASSERT_TEST(keymap[0].oldPinState == true && keymap[1].oldPinState == true);
}