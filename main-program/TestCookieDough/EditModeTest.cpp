#include "test.h"
#include "../lib/CookieDoughLibrary/EditMode.h"
#include "Fakes/Arduino.h"

const int normalKeyCount = 2;

extern uint8_t digitalWrite_param_pin;
extern uint8_t digitalWrite_param_val;

void EditMode_Initialized_NotEnabledByDefault() // TODO: Write a few test for EditMode using class mock to test that it's working. Then create a new testSuite_WORKING.txt Test.
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
    Key expected2 = Key(1337, 420);
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
    int expectedPin = 23;
    int expectedKeyCode = 3;
    Key keymap[normalKeyCount] = {
        Key(expectedPin, 1),
        Key(2, expectedKeyCode),
    };

    em.CopyKeyMapToTemporary(keymap);
    keymap[0].pin = 983;
    keymap[1].keyCode = 8787;
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

// TODO: Continue testing "EditModeLoop".