#include "test.h"
#include "../lib/CookieDoughLibrary/EditMode.h"
#include "Fakes/Arduino.h"

const int normalKeyCount = 2;

extern unsigned int digitalWrite_invocations;
extern uint8_t digitalWrite_param_pin;
extern uint8_t digitalWrite_param_val;

extern double pow_return;
extern unsigned int pow_invocations;
extern double pow_param_base;
extern double pow_param_exponent;

extern unsigned long millis_return;
extern unsigned int millis_invocations;

void EditMode_Initialized_NotEnabledByDefault()
{
    EditMode editmode = EditMode(true);

    ASSERT_TEST(editmode.enabled == false);
}

void Toggle_WhenDisabled_BecomesEnabled()
{
    EditMode editmode = EditMode(true);
    editmode.enabled = false;

    editmode.Toggle();

    ASSERT_TEST(editmode.enabled == true);
}

void Toggle_WhenEnabled_BecomesDisabled()
{
    EditMode editmode = EditMode(true);
    editmode.enabled = true;

    editmode.Toggle();

    ASSERT_TEST(editmode.enabled == false);
}

void Reset_SelectedKeyNotReset_BecomesNullptr()
{
    EditMode editmode = EditMode(true);
    Key selectedKey = Key(24, 1337);
    editmode.selectedKey = &selectedKey;

    editmode.Reset();

    ASSERT_TEST(editmode.selectedKey == nullptr);
}

void Reset_KeysPressedNotReset_BecomesZero()
{
    EditMode editmode = EditMode(true);
    editmode.keysPressed = 1337;

    editmode.Reset();

    ASSERT_TEST(editmode.keysPressed == 0);
}

void Reset_InputKeyCodeNotReset_BecomesZero()
{
    EditMode editmode = EditMode(true);
    editmode.inputKeyCode = 666;

    editmode.Reset();

    ASSERT_TEST(editmode.inputKeyCode == 0);
}

void Reset_ShouldAddValueNotReset_BecomesFalse()
{
    EditMode em = EditMode(true);
    em.shouldAddValue = true;

    em.Reset();

    ASSERT_TEST(em.shouldAddValue == false);
}

void Reset_BlinkValuesNotReset_BecomesResetToInitialValues()
{
    EditMode em = EditMode(true);
    em.ledIsOn = true;
    em.nextBlinkCycle = 1337;
    em.nextBlinkCycleOff = 1338;
    em.currentBlink = 1339;

    em.Reset();

    ASSERT_TEST(em.ledIsOn == false &&
                em.nextBlinkCycle == 0 &&
                em.nextBlinkCycleOff == 0 &&
                em.currentBlink == 0);
}

void Reset_BuiltinLedIsOn_IsTurnedOff()
{
    EditMode em = EditMode(true);

    em.Reset();

    ASSERT_TEST(digitalWrite_param_pin == LED_BUILTIN && digitalWrite_param_val == LOW);
}

void CopyKeyMapToTemporary_TempCopyIsOverwrittenWithTheValuesOfTheProvidedKeyMap()
{
    EditMode em = EditMode(true);
    Key expected1 = Key(1, 1);
    Key expected2 = Key(37, 420);
    Key keymap[normalKeyCount] = {
        expected1,
        expected2,
    };

    em.CopyKeyMapToTemporary(keymap);

    bool firstKeysAreSame = em.tempCopy[0].pin == expected1.pin && em.tempCopy[0].keyCode == expected1.keyCode;
    bool secondKeysAreSame = em.tempCopy[1].pin == expected2.pin && em.tempCopy[1].keyCode == expected2.keyCode;
    ASSERT_TEST(firstKeysAreSame && secondKeysAreSame);
}

void RestoreKeyMapFromTemporaryCopy_ProvidedKeyMapIsRestoredToStateWhenCopyKeyMapToTemporaryWasCalled()
{
    EditMode em = EditMode(true);
    uint8_t expectedPin = 23;
    int expectedKeyCode = 3;
    Key keymap[normalKeyCount] = {
        Key(expectedPin, 1),
        Key(2, expectedKeyCode),
    };

    em.CopyKeyMapToTemporary(keymap);
    keymap[0].pin = 98;
    keymap[1].keyCode = 87;
    em.RestoreKeyMapFromTemporaryCopy(keymap);

    ASSERT_TEST(keymap[0].pin == expectedPin && keymap[1].keyCode == expectedKeyCode);
}

void RestoreKeyMapFromTemporaryCopy_ResetEditMode()
{
    EditMode em = EditMode(true);
    Key keymap[normalKeyCount] = {Key(1, 1), Key(2, 2)};
    em.selectedKey = &keymap[0];

    em.RestoreKeyMapFromTemporaryCopy(keymap);

    ASSERT_TEST(em.selectedKey == nullptr);
}

void RegisterKeyPress_AddsOneToKeysPressed()
{
    EditMode em = EditMode(true);
    Key key = Key(1, 2);
    em.keysPressed = 0;
    int expectedKeysPressed = 1;

    em.RegisterKeyPress(&key);

    ASSERT_TEST(em.keysPressed == expectedKeysPressed);
}

void RegisterKeyPress_IfNoKeyIsSelected_SelectProvidedKey()
{
    EditMode em = EditMode(true);
    Key expectedSelectedKey = Key(1, 2);

    em.RegisterKeyPress(&expectedSelectedKey);

    ASSERT_TEST(em.selectedKey == &expectedSelectedKey);
}

void RegisterKeyPress_IfKeyHasAlreadyBeenSelected_DoNotUpdateSelectedKey()
{
    EditMode em = EditMode(true);
    Key expectedSelectedKey = Key(1, 2);
    Key anotherKey = Key(23, 32);
    em.selectedKey = &expectedSelectedKey;

    em.RegisterKeyPress(&anotherKey);

    ASSERT_TEST(em.selectedKey == &expectedSelectedKey);
}

void RegisterKeyPress_TheFirstKeyIsBeingPressed_ShouldNotPrepareToAddValueToKey()
{
    EditMode em = EditMode(true);
    Key key = Key(1, 2);
    em.shouldAddValue = false;
    em.keysPressed = 0;

    em.RegisterKeyPress(&key);

    ASSERT_TEST(em.shouldAddValue == false);
}

void RegisterKeyPress_FirstKeyHasAlreadyBeenRegistered_PrepareToAddValueToTheKey()
{
    EditMode em = EditMode(true);
    Key key = Key(1, 2);
    em.shouldAddValue = false;
    em.keysPressed = 1;

    em.RegisterKeyPress(&key);

    ASSERT_TEST(em.shouldAddValue == true);
}

void RegisterKeyRelease_KeysPressedIsDecreasedByOne()
{
    EditMode em = EditMode(true);
    em.keysPressed = 1;

    em.RegisterKeyRelease();

    ASSERT_TEST(em.keysPressed == 0);
}

void RegisterKeyRelease_SelectedKeyIsNullptrAndOneKeyIsPressed_PreventsAccessingSelectedKeyWhenNullptr()
{
    EditMode em = EditMode(true);
    bool didNotCrash = false;
    em.selectedKey = nullptr;
    em.keysPressed = 1;

    em.RegisterKeyRelease();
    didNotCrash = true;

    ASSERT_TEST(didNotCrash == true);
}

void RegisterKeyRelease_ShouldAddValueIsTrue_InputKeyCodeIsChanged()
{
    EditMode em = EditMode(true);
    em.shouldAddValue = true;
    em.inputKeyCode = 0;
    pow_return = 1337;

    em.RegisterKeyRelease();

    ASSERT_TEST(em.inputKeyCode != 0);
}

void RegisterKeyRelease_ShouldAddValueIsTrue_CorrectValueIsAddedToInputKeyCode()
{
    EditMode em = EditMode(true);
    em.keysPressed = 3;
    em.shouldAddValue = true;
    em.inputKeyCode = 5;
    pow_return = 10;
    int expectedKeyCode = 15;

    em.RegisterKeyRelease();

    ASSERT_TEST(em.inputKeyCode == expectedKeyCode);
}

void RegisterKeyRelease_TwoKeysWerePressedBeforeReleasing_ProvidesExponentZeroAndBaseTenToPow()
{
    EditMode em = EditMode(true);
    em.shouldAddValue = true;
    em.keysPressed = 2;

    em.RegisterKeyRelease();

    ASSERT_TEST(pow_param_exponent == 0 && pow_param_base == 10);
}

void RegisterKeyRelease_ThreeKeysWerePressedBeforeReleasing_ProvidesExponentOneAndBaseTenToPow()
{
    EditMode em = EditMode(true);
    em.shouldAddValue = true;
    em.keysPressed = 3;

    em.RegisterKeyRelease();

    ASSERT_TEST(pow_param_exponent == 1 && pow_param_base == 10);
}

void RegisterKeyRelease_FourKeysWerePressedBeforeReleasing_ProvidesExponentTwoAndBaseTenToPow()
{
    EditMode em = EditMode(true);
    em.shouldAddValue = true;
    em.keysPressed = 4;

    em.RegisterKeyRelease();

    ASSERT_TEST(pow_param_exponent == 2 && pow_param_base == 10);
}

void RegisterKeyRelease_AfterAddingValue_PreventNextKeyReleaseFromAddingValueToInputKeyCode()
{
    EditMode em = EditMode(true);
    em.shouldAddValue = true;

    em.RegisterKeyRelease();

    ASSERT_TEST(em.shouldAddValue == false);
}

void RegisterKeyRelease_LastKeyIsReleased_AppliesInputKeyCodeToKeyBeingEdited()
{
    EditMode em = EditMode(true);
    Key keyBeingEdited = Key(1, 0);
    em.selectedKey = &keyBeingEdited;
    em.keysPressed = 1;
    em.shouldAddValue = false;
    em.inputKeyCode = 1337;
    int expectedKeyCode = 1337;

    em.RegisterKeyRelease();

    ASSERT_TEST(keyBeingEdited.keyCode == 1337);
}

void RegisterKeyRelease_LastKeyIsReleased_ResetsUsedVariables()
{
    EditMode em = EditMode(true);
    Key key = Key(1, 0);
    em.selectedKey = &key;
    em.keysPressed = 1;
    em.shouldAddValue = false;

    em.RegisterKeyRelease();

    ASSERT_TEST(em.inputKeyCode == 0 && em.selectedKey == nullptr && em.keysPressed == 0 && em.shouldAddValue == false);
}

void RegisterKeyRelease_ShouldNotAddValue_InputKeyCodeRemainsTheSame()
{
    EditMode em = EditMode(true);
    pow_return = 13;
    em.shouldAddValue = false;
    em.inputKeyCode = 5;

    em.RegisterKeyRelease();

    ASSERT_TEST(em.inputKeyCode == 5);
}

void EditModeLoop_KeyHasNotBeenChanged_DoesNotDoAnything()
{
    EditMode em = EditMode(true);
    Key keymap[normalKeyCount] = {Key(1, 2), Key(3, 4)};
    keymap[0].value = keymap[0].oldValue = false;
    keymap[1].value = keymap[1].oldValue = false;
    em.keysPressed = 0;

    em.EditModeLoop(keymap);

    ASSERT_TEST(em.keysPressed == 0);
}

void EditModeLoop_KeyBecamePressed_RegistersKeyPress()
{
    EditMode em = EditMode(true);
    Key keymap[normalKeyCount] = {Key(1, 2), Key(3, 4)};
    keymap[0].value = keymap[0].oldValue = false;
    keymap[1].value = keymap[1].oldValue = false;
    keymap[0].value = true;
    em.keysPressed = 0;

    em.EditModeLoop(keymap);

    ASSERT_TEST(em.keysPressed == 1);
}

void EditModeLoop_TwoKeysBecamePressed_RegistersTwoKeyPresses()
{
    EditMode em = EditMode(true);
    Key keymap[normalKeyCount] = {Key(1, 2), Key(3, 4)};
    keymap[0].value = keymap[0].oldValue = false;
    keymap[1].value = keymap[1].oldValue = false;
    keymap[0].value = true;
    keymap[1].value = true;
    em.keysPressed = 0;

    em.EditModeLoop(keymap);

    ASSERT_TEST(em.keysPressed == 2);
}

void EditModeLoop_KeyBecameReleased_RegistersKeyRelease()
{
    EditMode em = EditMode(true);
    Key keymap[normalKeyCount] = {Key(1, 2), Key(3, 4)};
    keymap[0].value = keymap[0].oldValue = false;
    keymap[1].value = keymap[1].oldValue = false;
    keymap[0].oldValue = true;
    em.keysPressed = 1;

    em.EditModeLoop(keymap);

    ASSERT_TEST(em.keysPressed == 0);
}

void EditModeLoop_TwoKeysBecameReleased_RegistersTwoKeyReleases()
{
    EditMode em = EditMode(true);
    Key keymap[normalKeyCount] = {Key(1, 2), Key(3, 4)};
    keymap[0].value = keymap[0].oldValue = false;
    keymap[1].value = keymap[1].oldValue = false;
    keymap[0].oldValue = true;
    keymap[1].oldValue = true;
    em.keysPressed = 2;

    em.EditModeLoop(keymap);

    ASSERT_TEST(em.keysPressed == 0);
}

void EditModeLoop_WhenIdle_SignalEditMode()
{
    EditMode em = EditMode(true);
    Key keymap[normalKeyCount] = {Key(1, 2), Key(3, 4)};
    keymap[0].value = keymap[0].oldValue = false;
    keymap[1].value = keymap[1].oldValue = false;
    em.keysPressed = 0;
    em.useEditModeLedSignal = true;
    
    em.nextBlinkCycleOff = 0;
    em.ledIsOn = true;
    millis_return = em.nextBlinkCycleOff + 100;
    
    em.EditModeLoop(keymap);

    ASSERT_TEST(digitalWrite_invocations != 0 && digitalWrite_param_pin == LED_BUILTIN);
}