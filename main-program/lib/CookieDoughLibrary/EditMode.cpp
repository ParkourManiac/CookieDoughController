#include "EditMode.h"
#include <Arduino.h>

EditMode::EditMode(const EditMode& other) 
    :EditMode(other.normalKeyCount, other.useEditModeLedSignal)
{
}

EditMode::EditMode(int _normalKeyCount, bool _useEditModeLedSignal)
    : useEditModeLedSignal(_useEditModeLedSignal), normalKeyCount(_normalKeyCount), selectedKey(nullptr), tempCopy(new Key[normalKeyCount])
{
}

EditMode::~EditMode()
{
    delete[](tempCopy);
}

void EditMode::Toggle()
{
    enabled = !enabled;
    Reset();
}

void EditMode::CopyKeyMapToTemporary(Key *keyMap)
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        tempCopy[i] = keyMap[i];
    }
}

void EditMode::RestoreKeyMapFromTemporaryCopy(Key *keyMapToRestore)
{
    DEBUG(
        DEBUG_PRINT(F("\n"));
        DEBUG_PRINT(F("Applying temp to current keymap...\n"));
        for (int i = 0; i < normalKeyCount; i++) {
            DEBUG_PRINT(F("Temp .pin "));
            DEBUG_PRINT(tempCopy[i].pin);
            DEBUG_PRINT(F(", .keyCode "));
            DEBUG_PRINT(tempCopy[i].keyCode);
            DEBUG_PRINT(F(" -> "));

            DEBUG_PRINT(F("Current .pin "));
            DEBUG_PRINT(keyMapToRestore[i].pin);
            DEBUG_PRINT(F(", .keyCode "));
            DEBUG_PRINT(keyMapToRestore[i].keyCode);
            DEBUG_PRINT(F(".\n"));
        } DEBUG(delay(100)););

    for (int i = 0; i < normalKeyCount; i++)
    {
        keyMapToRestore[i] = tempCopy[i];
    }
    Reset();

    DEBUG(
        DEBUG_PRINT(F("Current keymap reset to:\n"));
        for (int i = 0; i < normalKeyCount; i++) {
            DEBUG_PRINT(F("Current .pin = "));
            DEBUG_PRINT(keyMapToRestore[i].pin);
            DEBUG_PRINT(F(", .keyCode = "));
            DEBUG_PRINT(keyMapToRestore[i].keyCode);
            DEBUG_PRINT(F("\n"));
        } DEBUG(delay(100)););
}

void EditMode::EditModeLoop(Key *keyMapBeingEdited)
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        Key &key = keyMapBeingEdited[i];

        if (OnKeyPress(key.state))
        {
            digitalWrite(LED_BUILTIN, HIGH);
            RegisterKeyPress(&key);
        }
        else if (OnKeyRelease(key.state))
        {
            DEBUG_PRINT(F("Keyrelease.\n")); // DEBUG
            digitalWrite(LED_BUILTIN, LOW);

            RegisterKeyRelease();
        }
    }

    // Signal that we are in edit mode.
    if (keysPressed == 0 && useEditModeLedSignal)
    {
        SignalLedEditMode();
    }
}

void EditMode::RegisterKeyPress(Key *pressedKey)
{
    keysPressed += 1;

    DEBUG_PRINT(F("Keypress.\n"));    // DEBUG
    DEBUG_PRINT(F("Keys pressed: ")); // DEBUG
    DEBUG_PRINT(keysPressed);      // DEBUG
    DEBUG_PRINT(F("\n"));

    if (selectedKey == nullptr)
    {
        selectedKey = pressedKey;

        DEBUG_PRINT(F("Selected key: ")); // DEBUG
        DEBUG_PRINT(selectedKey->pin); // DEBUG
        DEBUG_PRINT(F("\n"));
    }

    if (keysPressed > 1)
    {
        // If two or more keys are held down we should add a value...
        shouldAddValue = true;
    }
}

void EditMode::RegisterKeyRelease()
{
    keysPressed -= 1;

    if (shouldAddValue)
    {
        // Raise value of keycode.
        int exponent = keysPressed - 1;
        int numberToAdd = static_cast<int>(pow(10, exponent));
        inputKeyCode += numberToAdd;

        shouldAddValue = false;

        DEBUG(
            if (selectedKey != nullptr)
            {
                DEBUG_PRINT(F("Inputed keycode: "));
                DEBUG_PRINT(inputKeyCode);
                DEBUG_PRINT(F(", (for pin: "));
                DEBUG_PRINT(selectedKey->pin);
                DEBUG_PRINT(F(", keycode: "));
                DEBUG_PRINT(selectedKey->keyCode);
                DEBUG_PRINT(F(")\n"));
            }
            else 
            {
                DEBUG_PRINT(F("Selected is nullptr!\n"));
            }
        );
    }

    // If we are releasing the last pressed key...
    if (keysPressed == 0 && selectedKey != nullptr)
    {
        selectedKey->keyCode = inputKeyCode;

        // DEBUG
        DEBUG_PRINT(F("Updated key: .pin = "));
        DEBUG_PRINT(selectedKey->pin);
        DEBUG_PRINT(F(", .keyCode = "));
        DEBUG_PRINT(selectedKey->keyCode);
        DEBUG_PRINT(F("\n"));
        // DEBUG

        Reset();
    }

    DEBUG_PRINT(F("Amount of keys pressed: ")); // DEBUG
    DEBUG_PRINT(keysPressed);                // DEBUG
    DEBUG_PRINT(F("\n"));
}

void EditMode::SignalLedEditMode() // Not tested.
{
    uint32_t currentTime = millis();

    // if its time to turn off led...
    if (ledIsOn)
    {
        if (nextBlinkCycleOff < currentTime)
        {
            digitalWrite(LED_BUILTIN, LOW);
            ledIsOn = false;
            nextBlinkCycle = currentTime + 100;
        }
    }
    else
    {
        // If its time to blink...
        if (nextBlinkCycle < currentTime)
        {
            // If we should pulse...
            if (currentBlink < blinksPerSignal)
            {
                digitalWrite(LED_BUILTIN, HIGH);
                ledIsOn = true;

                currentBlink++;
                nextBlinkCycle = currentTime + 200;
            }
            else // we are done pulsing...
            {
                currentBlink = 0;
                nextBlinkCycle = currentTime + 2000;
            }

            nextBlinkCycleOff = currentTime + 200;
        }
    }
}

void EditMode::Reset()
{
    // Reset
    selectedKey = nullptr;
    keysPressed = 0;
    inputKeyCode = 0;
    shouldAddValue = false;

    // Reset led signal
    nextBlinkCycle = 0;
    nextBlinkCycleOff = 0;
    currentBlink = 0;
    digitalWrite(LED_BUILTIN, LOW);
    ledIsOn = false;
}
