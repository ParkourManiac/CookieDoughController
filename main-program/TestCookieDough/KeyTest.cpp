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

void SpecialKeyEqualityOperator_PinAndFunctionAreTheSameBetweenObjects_ReturnsTrue()
{
    IKey pin = 1;
    SpecialFunction function = toggleDefaultKeyMap;
    SpecialKey first = SpecialKey(pin, function);
    SpecialKey second = SpecialKey(pin, function);
    bool expectedResult = true;

    bool result = first == second;

    ASSERT_TEST(result == expectedResult);
}

void SpecialKeyEqualityOperator_PinDifferBetweenObjects_ReturnsFalse()
{
    SpecialKey first = SpecialKey(5, toggleDefaultKeyMap);
    SpecialKey second = SpecialKey(1, toggleDefaultKeyMap);
    bool expectedResult = false;

    bool result = first == second;
    
    ASSERT_TEST(result == expectedResult);
}

void SpecialKeyEqualityOperator_FunctionDifferBetweenObjects_ReturnsFalse()
{
    SpecialKey first = SpecialKey(1, toggleDefaultKeyMap);
    SpecialKey second = SpecialKey(1, cycleKeyMap);
    bool expectedResult = false;

    bool result = first == second;
    
    ASSERT_TEST(result == expectedResult);
}

void SpecialKeyEqualityOperator_PinAndFunctionAreTheSameBetweenObjectsButStateIsDifferent_IgnoresStateAndReturnsTrue()
{
    IKey pin = 1;
    SpecialFunction function = toggleDefaultKeyMap;
    SpecialKey first = SpecialKey(pin, function);
    SpecialKey second = SpecialKey(pin, function);
    first.state.value = false;
    second.state.value = true;
    bool expectedResult = true;

    bool result = first == second;

    ASSERT_TEST(result == expectedResult);
}

void SpecialKeyNotEqualityOperator_PinAndFunctionDifferBetweenObjects_ReturnsOppositeOfEqualityOperator()
{
    SpecialKey first = SpecialKey(1, toggleDefaultKeyMap);
    SpecialKey second = SpecialKey(3, cycleKeyMap);
    bool expectedResult = !(first == second);

    bool result = first != second;

    ASSERT_TEST(result == expectedResult);
}

void SpecialKeyNotEqualityOperator_PinAndFunctionAreTheSameBetweenObjects_ReturnsOppositeOfEqualityOperator()
{
    SpecialKey first = SpecialKey(1, toggleDefaultKeyMap);
    SpecialKey second = SpecialKey(1, toggleDefaultKeyMap);
    bool expectedResult = !(first == second);

    bool result = first != second;

    ASSERT_TEST(result == expectedResult);
}

void ConfigurePinForKey_IKeyIsPassedToPinMode()
{
    IKey expectedPin = 2;

    ConfigurePinForKey(expectedPin);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinForKeyOfTypeKey_IsCorrectlyParsedToIKey()
{
    uint8_t expectedPin = 2;
    Key key = Key(expectedPin, 1337);

    ConfigurePinForKey(key.pin);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinForKeyOfTypeSpecialKey_IsCorrectlyParsedToIKey()
{
    uint8_t expectedPin = 2;
    SpecialKey key = SpecialKey(expectedPin, toggleDefaultKeyMap);

    ConfigurePinForKey(key.pin);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinForKey_ConfiguresPinAsInputPullup()
{
    IKey key = 2;

    ConfigurePinForKey(key);

    ASSERT_TEST(pinMode_param_mode == INPUT_PULLUP);
}

void ConfigurePinForKey_CallsPinModeOnce()
{
    SpecialKey key = SpecialKey(2, toggleDefaultKeyMap);

    ConfigurePinForKey(key.pin);

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

    DebounceReadState(0, &state);

    ASSERT_TEST(state.oldValue == true);
}

void DebounceRead_ReadsStateOfPin()
{
    IPinState state;
    IKey pin = 3;

    DebounceReadState(pin, &state);

    ASSERT_TEST(digitalRead_param_pin == pin && digitalRead_invocations == 1);
}

void DebounceRead_PinStateHasChanged_UpdatesLastDebounceTime()
{
    IPinState state;
    unsigned long expectedDebounceTime = 1337;
    millis_return = expectedDebounceTime;
    state.lastDebounceTime = 0;
    state.oldPinState = true;
    digitalRead_return = false;

    DebounceReadState(0, &state);

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

    DebounceReadState(0, &state);

    ASSERT_TEST(state.lastDebounceTime == expectedDebounceTime);
}

void DebounceRead_DebounceTimeExceededAndValueIsOutdated_UpdateStateValue()
{
    IPinState state;
    state.lastDebounceTime = 0;
    millis_return = state.lastDebounceTime + 100000;
    digitalRead_return = true; // true = Button is released.
    state.oldPinState = true;
    state.value = true;    // Button was previously pressed.
    bool expected = false; // We expect the button to be released.

    DebounceReadState(0, &state);

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

    DebounceReadState(0, &state);

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

    DebounceReadState(0, &state);

    ASSERT_TEST(state.value == expected);
}

void DebounceRead_DebounceTimeExceededAndTheValueIsActiveButValueIsNotOutdated_DoesNotUpdateTheTimeOfActivation()
{
    bool currentPinState = false; // false = Button is pressed.
    digitalRead_return = currentPinState;
    IPinState state;
    state.oldPinState = !currentPinState;
    state.value = true; // Button was previously released.
    millis_return = state.lastDebounceTime + 100000;
    unsigned long expectedTimeOfActivation = 1337;
    state.timeOfActivation = expectedTimeOfActivation;

    DebounceReadState(0, &state);

    ASSERT_TEST(state.timeOfActivation == expectedTimeOfActivation);
}

void DebounceRead_OldPinStateIsUpdated()
{
    IPinState state;
    digitalRead_return = false;       // Button is pressed.
    state.oldPinState = true;         // Button was released.
    bool expectedOldPinState = false; // We expect it to become released.

    DebounceReadState(0, &state);

    ASSERT_TEST(state.oldPinState == expectedOldPinState);
}

void UpdatePinStatesForKeyMap_CallsDigitalReadForEachItem()
{
    unsigned int length = 2;
    Key keymap[length] = {
        Key(2, 1337),
        Key(3, 1337),
    };

    UpdatePinStatesForKeyMap(keymap, length);

    ASSERT_TEST(digitalRead_invocations == length);
}

void UpdatePinStatesForKeyMap_CorrectlyParsesKeyPin()
{
    int length = 1;
    Key keymap[length] = {
        Key(3, 1337),
    };
    uint8_t expectedPin = 3;

    UpdatePinStatesForKeyMap(keymap, length);

    ASSERT_TEST(digitalRead_param_pin == expectedPin);
}

void UpdatePinStatesForKeyMap_UpdatesStateForAllPins()
{
    digitalRead_return = true;
    int length = 2;
    Key keymap[length] = {
        Key(2, 1337),
        Key(3, 1337),
    };

    UpdatePinStatesForKeyMap(keymap, length);

    ASSERT_TEST(keymap[0].state.oldPinState == true && keymap[1].state.oldPinState == true);
}

void UpdatePinStatesForKeyMap_KeymapUsesDatatypeKey_Works() 
{
    digitalRead_return = true;
    int length = 2;
    Key keymap[length] = {
        Key(2, 1337),
        Key(3, 1337),
    };
    keymap[0].state.oldPinState = keymap[1].state.oldPinState = false;

    UpdatePinStatesForKeyMap(keymap, length);

    ASSERT_TEST(keymap[0].state.oldPinState == true && keymap[1].state.oldPinState == true);
}

void UpdatePinStatesForKeyMap_KeymapUsesDatatypeSpecialKey_Works()
{
    digitalRead_return = true;
    int length = 2;
    SpecialKey keymap[length] = {
        SpecialKey(2, toggleDefaultKeyMap),
        SpecialKey(3, toggleEditMode),
    };
    keymap[0].state.oldPinState = keymap[1].state.oldPinState = false;

    UpdatePinStatesForKeyMap(keymap, length);

    ASSERT_TEST(keymap[0].state.oldPinState == true && keymap[1].state.oldPinState == true);
}

void KeyConstructor_IntializesPinAndKeycodeCorrectly()
{
    uint8_t expectedPin = 7;
    int expectedKeycode = 19;

    Key key = Key(expectedPin, expectedKeycode);

    ASSERT_TEST(key.pin == expectedPin && key.keyCode == expectedKeycode);
}

void SpecialKeyConstructor_IntializesPinAndFunctionCorrectly()
{
    uint8_t expectedPin = 7;
    SpecialFunction expectedFunction = cycleKeyMap;

    SpecialKey key = SpecialKey(expectedPin, expectedFunction);

    ASSERT_TEST(key.pin == expectedPin && key.function == expectedFunction);
}

void BareKeyboardKeyConstructor_IntializesPinAndKeycodeCorrectly()
{
    IKey expectedPin = 7;
    IKeycode expectedKeycode = 19;

    BareKeyboardKey key = BareKeyboardKey(expectedPin, expectedKeycode);

    ASSERT_TEST(key.pin == expectedPin && key.keyCode == expectedKeycode);
}