#include "EditMode.h"
#include <Arduino.h>

EditMode::EditMode(bool _useEditModeLedSignal)
{
    useEditModeLedSignal = _useEditModeLedSignal;
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
    // DEBUG
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Applying temp to current keymap...");
    for (int i = 0; i < normalKeyCount; i++)
    {
        DEBUG_PRINT("Temp .pin ");
        DEBUG_PRINT(tempCopy[i].pin);
        DEBUG_PRINT(", .keyCode ");
        DEBUG_PRINT(tempCopy[i].keyCode);
        DEBUG_PRINT(" -> ");

        DEBUG_PRINT("Current .pin ");
        DEBUG_PRINT(keyMapToRestore[i].pin);
        DEBUG_PRINT(", .keyCode ");
        DEBUG_PRINT(keyMapToRestore[i].keyCode);
        DEBUG_PRINTLN(".");
    }
    delay(100);
    // DEBUG

    for (int i = 0; i < normalKeyCount; i++)
    {
        keyMapToRestore[i] = tempCopy[i];
    }
    Reset();

    // DEBUG
    DEBUG_PRINTLN("Current keymap reset to:");
    for (int i = 0; i < normalKeyCount; i++)
    {
        DEBUG_PRINT("Current .pin = ");
        DEBUG_PRINT(keyMapToRestore[i].pin);
        DEBUG_PRINT(", .keyCode = ");
        DEBUG_PRINTLN(keyMapToRestore[i].keyCode);
    }
    delay(100);
    // DEBUG
}

// TODO: Check if working... Changed to OnKeyPress and refactored. Might not work? Tests go through. If working in play testing then remove this TODO.
void EditMode::EditModeLoop(Key *keyMapBeingEdited) 
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        Key &key = keyMapBeingEdited[i];

        if (OnKeyPress(key))
        {
            digitalWrite(LED_BUILTIN, HIGH);
            RegisterKeyPress(key);
        }
        else if (OnKeyRelease(key))
        {
            DEBUG_PRINTLN("Keyrelease."); // DEBUG
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

void EditMode::RegisterKeyPress(Key &pressedKey)
{
    keysPressed += 1;

    DEBUG_PRINTLN("Keypress.");    // DEBUG
    DEBUG_PRINT("Keys pressed: "); // DEBUG
    DEBUG_PRINTLN(keysPressed);    // DEBUG

    if (selectedKey == nullptr)
    {
        selectedKey = &pressedKey;

        DEBUG_PRINT("Selected key: ");   // DEBUG
        DEBUG_PRINTLN(selectedKey->pin); // DEBUG
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
        int numberToAdd = pow(10, exponent);
        inputKeyCode += numberToAdd;

        shouldAddValue = false;

        // DEBUG
        if (selectedKey != nullptr)
        {
            DEBUG_PRINT("Inputed keycode: ");
            DEBUG_PRINT(inputKeyCode);
            DEBUG_PRINT(", (for pin: ");
            DEBUG_PRINT(selectedKey->pin);
            DEBUG_PRINT(", keycode: ");
            DEBUG_PRINT(selectedKey->keyCode);
            DEBUG_PRINTLN(")");
        }
        else
            DEBUG_PRINTLN("Selected is nullptr!");
        // DEBUG
    }

    // If we are releasing the last pressed key...
    if (keysPressed == 0 && selectedKey != nullptr)
    {
        selectedKey->keyCode = inputKeyCode;

        // DEBUG
        DEBUG_PRINT("Updated key: .pin = ");
        DEBUG_PRINT(selectedKey->pin);
        DEBUG_PRINT(", .keyCode = ");
        DEBUG_PRINTLN(selectedKey->keyCode);
        // DEBUG

        Reset();
    }

    DEBUG_PRINT("Amount of keys pressed: "); // DEBUG
    DEBUG_PRINTLN(keysPressed);              // DEBUG
}

void EditMode::SignalLedEditMode() // Not tested. 
{
    unsigned long currentTime = millis();

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