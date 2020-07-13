#include "EditMode.h"
#include <Arduino.h>

EditMode::EditMode(bool _useEditModeLedSignal)
{
    useEditModeLedSignal = useEditModeLedSignal;
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
    Serial.println();
    Serial.println("Applying temp to current keymap...");
    for (int i = 0; i < normalKeyCount; i++)
    {
        Serial.print("Temp .pin ");
        Serial.print(tempCopy[i].pin);
        Serial.print(", .keyCode ");
        Serial.print(tempCopy[i].keyCode);
        Serial.print(" -> ");

        Serial.print("Current .pin ");
        Serial.print(keyMapToRestore[i].pin);
        Serial.print(", .keyCode ");
        Serial.print(keyMapToRestore[i].keyCode);
        Serial.println(".");
    }
    delay(100);
    // DEBUG

    for (int i = 0; i < normalKeyCount; i++)
    {
        keyMapToRestore[i] = tempCopy[i];
    }
    Reset();

    // DEBUG
    Serial.println("Current keymap reset to:");
    for (int i = 0; i < normalKeyCount; i++)
    {
        Serial.print("Current .pin = ");
        Serial.print(keyMapToRestore[i].pin);
        Serial.print(", .keyCode = ");
        Serial.println(keyMapToRestore[i].keyCode);
    }
    delay(100);
    // DEBUG
}

// DEBUG NOTE: Tested this far.
void EditMode::EditModeLoop(Key *keyMapBeingEdited) // TODO: Check if working... Changed to OnKeyPress and refactored. Might not work? Create tests for this.
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
            Serial.println("Keyrelease."); // DEBUG
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

    Serial.println("Keypress.");    // DEBUG
    Serial.print("Keys pressed: "); // DEBUG
    Serial.println(keysPressed);    // DEBUG

    if (selectedKey == nullptr)
    {
        selectedKey = &pressedKey;

        Serial.print("Selected key: ");   // DEBUG
        Serial.println(selectedKey->pin); // DEBUG
    }

    if (!shouldAddValue && keysPressed > 1)
    {
        // If two or more keys are held down we should add a value...
        shouldAddValue = true;
    }
}

void EditMode::RegisterKeyRelease()
{
    if (selectedKey != nullptr)
    {
        if (shouldAddValue)
        {
            // Raise value of keycode.
            int exponent = keysPressed - 2;
            int numberToAdd = pow(10, exponent);
            inputKeyCode += numberToAdd;

            shouldAddValue = false;

            // DEBUG
            Serial.print("Inputed keycode: ");
            Serial.print(inputKeyCode);
            Serial.print(", (for pin: ");
            Serial.print(selectedKey->pin);
            Serial.print(", keycode: ");
            Serial.print(selectedKey->keyCode);
            Serial.println(")");
            // DEBUG
        }
    }
    else
        Serial.println("Selected is nullptr!"); // DEBUG

    keysPressed -= 1;

    // If we are releasing the last pressed key...
    if (keysPressed <= 0)
    {
        selectedKey->keyCode = inputKeyCode;

        // DEBUG
        Serial.print("Updated key: .pin = ");
        Serial.print(selectedKey->pin);
        Serial.print(", .keyCode = ");
        Serial.println(selectedKey->keyCode);
        // DEBUG

        Reset();
    }

    Serial.print("Amount of keys pressed: "); // DEBUG
    Serial.println(keysPressed);              // DEBUG
}

void EditMode::SignalLedEditMode()
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