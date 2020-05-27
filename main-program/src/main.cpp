#include <Arduino.h>
#include <EEPROM.h>

#include <DataPacket.h>
#include <Key.h>
#include <LinkedList.h>
#include <LinkedList.cpp>

// HEADER
void SaveKeyMapsToMemory(LinkedList<Key *> keyMapList);
void LoadKeyMapsFromMemory(LinkedList<Key *> &keyMapList);
void ConfigurePinsAsKeys();
void CycleKeyMap();
void ChangeKeyMap(Key *keyMap);
void ReadPinValueForKeys();
void SendKeyInfo();
void ExecuteSpecialCommands();
void SaveControllerSettings();
void DeleteCurrentKeyMap();
void ToggleEditMode();
void ResetEditMode();
void CopyCurrentKeyMapToTemporary();
void ResetCurrentKeyMapToTemporaryCopy();
bool CreateNewKeyMap();
void EditMode();
void SignalLedEditMode();
void DebounceRead(IPinState &key);
bool OnKeyPress(IPinState &key);
bool OnKeyRelease(IPinState &key);
void SignalErrorToUser();

// Public variables
const int normalKeyCount = 4;

Key defaultKeyMap[normalKeyCount] = {
    // Key map Arrow keys
    Key(2, 80),
    Key(3, 82),
    Key(4, 81),
    Key(5, 79),
};

SpecialKey specialKeys[3] = {
    SpecialKey(10, toggleEditMode),
    SpecialKey(11, cycleKeyMap),
    SpecialKey(12, toggleDefaultKeyMap), // This one should never change.
};

Key *currentKeyMap = defaultKeyMap;
unsigned int customKeyMapIndex = 0;
LinkedList<Key *> *customKeyMapsPtr = new LinkedList<Key *>();
LinkedList<Key *> customKeyMaps = *customKeyMapsPtr;

uint8_t buf[8] = {0}; // Keyboard report buffer.

unsigned int eepromAdress = 0;
unsigned int nextFreeEepromAdress = 0;

bool editmode = false;
Key *editmodeSelectedKey = nullptr;
Key *editmodeTempCopy = new Key[normalKeyCount];
int editmodeKeysPressed = 0;
int editmodeKeyCode = 0;
bool editmodeShouldAddValue = false;

const int editmodeBlinksPerSignal = 3;
bool editmodeLedIsOn = false;
unsigned long editmodeNextBlinkCycle = 0;
unsigned long editmodeNextBlinkCycleOff = 0;
int editmodeCurrentBlink = 0;

const float longPressDuration = 4000;

// PROGRAM
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    while (!Serial)
    {
        // Wait for serial port to start.
    }

    // for(unsigned int i = 0; i < EEPROM.length(); i++) {
    //     EEPROM.write(i, 0);
    // }
    // Key keys[normalKeyCount] = {
    //     Key(2, 4),
    //     Key(3, 26),
    //     Key(4, 22),
    //     Key(5, 7),
    // };
    // nextFreeEepromAdress = 50;
    // customKeyMaps.Add(keys);
    // SaveKeyMapsToMemory(customKeyMaps);

    customKeyMaps.Clear();
    LoadKeyMapsFromMemory(customKeyMaps);
    ConfigurePinsAsKeys();

    // // DEBUG
    // Serial.println();
    // Serial.println("New current keymap:");
    // for(int i = 0; i < normalKeyCount; i++) {
    //     Serial.print("Current .pin = ");
    //     Serial.print(currentKeyMap[i].pin);
    //     Serial.print(", .keyCode = ");
    //     Serial.println(currentKeyMap[i].keyCode);
    // }
    // delay(100);

    // for (unsigned int i = 0; i < customKeyMaps.length; i++)
    // {
    //     Serial.println("{");
    //     for (unsigned int j = 0; j < normalKeyCount; j++)
    //     {
    //         Serial.print("    .pin: ");
    //         Serial.println((*customKeyMaps[i])[j].pin);
    //         Serial.print("    .keyCode: ");
    //         Serial.println((*customKeyMaps[i])[j].keyCode);
    //     }
    //     Serial.println("}");
    // }
    // // DEBUG
}

void loop()
{
    ReadPinValueForKeys();
    ExecuteSpecialCommands();
    if (editmode)
    {
        EditMode();
    }
    else
    {
        SendKeyInfo();
    }
}

/**
 * @brief Saves a list of keymaps to memory.
 * Note: Will overwrite existing keymaps on the eeprom.
 * 
 * @param keyMapList The list of keymaps to be saved.
 */
void SaveKeyMapsToMemory(LinkedList<Key *> keyMapList) // TODO: Needs to be tested.
{
    unsigned int serializedSize = sizeof(BareKeyboardKey[keyMapList.length * normalKeyCount]);
    // Key *serializedKeyMaps = new Key[keyMapList.length * normalKeyCount];
    BareKeyboardKey *serializedKeyMaps = new BareKeyboardKey[keyMapList.length * normalKeyCount];
    for (unsigned int i = 0; i < keyMapList.length; i++)
    {
        for (unsigned int j = 0; j < normalKeyCount; j++)
        {
            unsigned int pos = i * normalKeyCount + j;
            serializedKeyMaps[pos] = (BareKeyboardKey)(*keyMapList[i])[j];
        }
    }

    uint8_t *dataPtr = (uint8_t *)serializedKeyMaps;
    // // DEBUG
    // Serial.print("Passed in: ");
    // for(int i = 0; i < serializedSize; i++) {
    //     Serial.print(dataPtr[i], HEX);
    // }
    // Serial.println();
    // delay(100);
    // // DEBUG
    unsigned int packetSize;
    // TODO: Change this to write to nextFreeEepromAdress and invalidate the old one at eepromAdress.
    bool success = SavePacketToEEPROM(eepromAdress, dataPtr, serializedSize, packetSize);
    if (!success)
    {
        // Serial.println("Failed to write data to memory!");
        // delay(100);

        // TODO: Implement error code.
    }
    nextFreeEepromAdress += packetSize;

    delay(500);
    delete (serializedKeyMaps);
}

/**
 * @brief Loads the list of keymaps from memory
 * into the provided keyMap list.
 * 
 * @param keyMapList The keyMap list to store the result.
 */
void LoadKeyMapsFromMemory(LinkedList<Key *> &keyMapList)
{
    unsigned int packetAdress = 0;
    DataPacket *dataPtr = new DataPacket();
    DataPacket packet = *dataPtr;
    unsigned int packetSize;

    bool foundPacket = false;
    do
    {
        foundPacket = ParsePacketFromEEPROM(packetAdress, packet, packetSize);
        if (!foundPacket)
            packetAdress++;

        if (packetAdress >= EEPROM.length())
        {
            // Serial.println("Failed to read data from memory!");
            // delay(100);

            // TODO: Implement error code.
            return;
        }
    } while (!foundPacket && packetAdress < EEPROM.length());

    // Serial.println("Began loading...");
    // delay(100);

    // Convert
    unsigned int amountOfKeys = packet.payloadLength / sizeof(BareKeyboardKey);
    BareKeyboardKey payloadAsBareKeys[normalKeyCount * amountOfKeys];
    for (unsigned int i = 0; i < amountOfKeys; i++)
    {
        payloadAsBareKeys[i] = ((BareKeyboardKey *)packet.payload)[i];
    }

    unsigned int amountOfKeymaps = amountOfKeys / normalKeyCount;
    for (unsigned int i = 0; i < amountOfKeymaps; i++) // For each keymap
    {
        Key *keyMap = new Key[normalKeyCount];
        for (unsigned int j = 0; j < normalKeyCount; j++) // For each key in a keymap
        {
            BareKeyboardKey currentKey = payloadAsBareKeys[i * normalKeyCount + j];
            keyMap[j].pin = currentKey.pin;
            keyMap[j].keyCode = currentKey.keyCode;
        }
        keyMapList.Add(keyMap);
    }

    // // DEBUG
    // Serial.println("Data:");
    // for (unsigned int i = 0; i < keyMapList.length; i++)
    // {
    //     Serial.println("{");
    //     for (unsigned int j = 0; j < normalKeyCount; j++)
    //     {
    //         Serial.print("    .pin: ");
    //         Serial.println((*keyMapList[i])[j].pin);
    //         Serial.print("    .keyCode: ");
    //         Serial.println((*keyMapList[i])[j].keyCode);
    //     }
    //     Serial.println("}");
    // }
    // delay(100);
    // // DEBUG

    // Serial.println("DATA:::::");
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     Serial.println(packet.payload[i], HEX);
    // }
    // Serial.println(":::::");

    // Serial.println("Finished loading.");

    // Serial.println();
    // Serial.print("Packet size: ");
    // Serial.println(packetSize);
    // delay(100);

    eepromAdress = packetAdress;
    nextFreeEepromAdress = packetAdress + packetSize;
    delete (dataPtr);
}

/**
 * @brief Configures pins marked as Key or SpecialKey to act as input pins with internal pullups.
 */
void ConfigurePinsAsKeys()
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        Key &key = currentKeyMap[i];
        pinMode(key.pin, INPUT_PULLUP);
    }

    for (SpecialKey &specialKey : specialKeys)
    {
        pinMode(specialKey.pin, INPUT_PULLUP);
    }
}

/**
 * @brief Switches to the next keyMap configuration in the list
 * of available keyMaps.
 * Note: If we are using the default keyMap then it will 
 * switch back to the previous keyMap instead of moving
 * to the next keyMap in the list.
 * 
 */
void CycleKeyMap()
{
    if (customKeyMaps.IsEmpty())
    {
        // We can't cycle through an empty list of keymaps...
        SignalErrorToUser();
        return;
    }

    // If we are using the default. Switch back to the
    // previous keymap. Otherwise move to the next.
    bool isDefault = (currentKeyMap == defaultKeyMap);
    int nextIndex = (isDefault) ? customKeyMapIndex : customKeyMapIndex + 1;

    customKeyMapIndex = nextIndex % customKeyMaps.length;
    Key **nextKeyMapPtr = customKeyMaps[customKeyMapIndex];

    if (nextKeyMapPtr != nullptr)
    {
        Key *nextKeyMap = *(nextKeyMapPtr);
        ChangeKeyMap(nextKeyMap);
    }
}

/**
 * @brief Changes the current keymap to the keymap specified at
 * the given index (in the available keyMaps).
 * 
 * @param index The index of the keymap to be switched to.
 */
void ChangeKeyMap(Key *keyMap)
{
    currentKeyMap = keyMap;
    ConfigurePinsAsKeys();
}

/**
 * @brief Switches to the built in default keyMap.
 */
void ToggleDefaultKeyMap()
{
    bool toggleToDefault = currentKeyMap != defaultKeyMap;
    if (toggleToDefault)
    {
        ChangeKeyMap(defaultKeyMap);
    }
    else if (customKeyMaps.IsEmpty())
    {
        // We can't toggle to custom if the list is empty.
        SignalErrorToUser();
    }
    else
    {
        // Toggle to custom keymap.
        Key **lastKeyMapPtr = customKeyMaps[customKeyMapIndex];
        if (lastKeyMapPtr != nullptr)
        {
            ChangeKeyMap(*(lastKeyMapPtr));
        }
    }
}

/**
 * @brief Reads and updates the pin values of
 *  the current keymap and special keys.
 */
void ReadPinValueForKeys()
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        Key &key = currentKeyMap[i];
        //key.value = !digitalRead(key.pin); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
        DebounceRead(key);
    }

    for (SpecialKey &specialKey : specialKeys) // TODO: Handle debounce.
    {
        DebounceRead(specialKey); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    }
}

/**
 * @brief Writes the keypress events to the buffer and sends them to the computer. 
 */
void SendKeyInfo() 
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        Key &key = currentKeyMap[i];

        if (OnKeyPress(key))
        {
            digitalWrite(LED_BUILTIN, HIGH);

            // Find empty slot
            int index = 2; // 2 = Start position for keys.
            bool foundEmpty = false;
            for (int i = 2; i < 8 && !foundEmpty; i++) // TODO: FORLOOP MIGHT NOT BE EFFICIENT.
            {
                foundEmpty = buf[i] == 0;
                if (foundEmpty)
                    index = i;
            }

            if (foundEmpty)
            {
                // Send keypress
                buf[index] = key.keyCode;
                Serial.write(buf, 8);
            }
        }
        else if(OnKeyRelease(key))
        {
            digitalWrite(LED_BUILTIN, LOW);

            // Find empty slot
            int index = 2; // 2 = Start position for keys.
            bool foundKeyCode = false;
            for (int i = 2; i < 8 && !foundKeyCode; i++) // TODO: FORLOOP MIGHT NOT BE EFFICIENT.
            {
                foundKeyCode = buf[i] == key.keyCode;
                if (foundKeyCode)
                    index = i;
            }

            if (foundKeyCode)
            {
                // Remove keycode from buffer
                buf[index] = 0;

                bool bufIsEmpty = true;
                for (int i = 2; i < 8; i++) // TODO: FORLOOP MIGHT NOT BE EFFICIENT.
                {
                    if (buf[i] != 0)
                    {
                        bufIsEmpty = false;
                        break;
                    }
                }

                if (bufIsEmpty)
                    buf[0] = 0;

                // Send release keypress
                Serial.write(buf, 8);
            }
        }
    }
}

/**
 * @brief Executes the corresponding special function when a special key is pressed.
 */
void ExecuteSpecialCommands()
{
    for (SpecialKey &specialKey : specialKeys)
    {
        if (specialKey.oldValue != specialKey.value)
        {
            if (specialKey.value)
            {
                digitalWrite(LED_BUILTIN, HIGH);

                // Activate corresponding function
                switch (specialKey.function)
                {
                case toggleEditMode:
                {
                    // See below. Toggled on release.
                    break;
                }
                case cycleKeyMap:
                {
                    if (editmode)
                    {
                        ToggleEditMode(); // Exit editmode to save the keyMap.
                        bool success = CreateNewKeyMap();
                        if (success)
                        {
                            ToggleEditMode();
                        }
                    }
                    else
                    {
                        CycleKeyMap();
                    }
                    break;
                }
                case toggleDefaultKeyMap:
                {
                    if (editmode)
                        ResetCurrentKeyMapToTemporaryCopy();
                    else
                        ToggleDefaultKeyMap();
                    break;
                }
                }
            }
            else // If we released the button...
            {
                digitalWrite(LED_BUILTIN, LOW);

                switch (specialKey.function)
                {
                case toggleEditMode:
                {
                    bool wasALongPress = false;
                    if (editmode)
                    {
                        wasALongPress = (millis() - specialKey.timeOfActivation) > longPressDuration;
                        // if we did a long press in editmode...
                        if (wasALongPress)
                        {
                            // Serial.println("Long press, released. Save to memory..."); // DEBUG
                            SaveControllerSettings();
                        }
                    }

                    ToggleEditMode();
                    break;
                }
                case cycleKeyMap:
                {
                    break;
                }
                case toggleDefaultKeyMap:
                {
                    if (editmode)
                    {
                        bool wasALongPress = (millis() - specialKey.timeOfActivation) > longPressDuration;
                        if (wasALongPress)
                        {
                            // Serial.println("Long press, released. Delete keymap..."); // DEBUG
                            DeleteCurrentKeyMap();
                        }
                    }
                    break;
                }
                }

                //Serial.println((millis() - specialKey.timeOfActivation)); // DEBUG
            }
        }
    }
}

void SaveControllerSettings()
{
    SaveKeyMapsToMemory(customKeyMaps);

    unsigned long timeNeeded = customKeyMaps.length * normalKeyCount * sizeof(BareKeyboardKey) * 5;

    // // DEBUG
    // Serial.print("Time needed to save: ");
    // Serial.println(timeNeeded);
    // // DEBUG

    // Signal that we are saving. Loop will wait for 800ms total.
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
    }

    // Wait remaining time
    if (timeNeeded > 800)
    {
        delay(timeNeeded - 800);
    }

    delay(200); // Overshoot with 200ms.
    digitalWrite(LED_BUILTIN, LOW);
}

void DeleteCurrentKeyMap()
{
    if (!editmode)
        return;
    if (customKeyMaps.IsEmpty())
        return;
    if (currentKeyMap == defaultKeyMap)
        return;
    // If we did not return above:
    // We are in editmode, we have atleast one keymap
    // in our list and we are not trying to
    // delete the default keymap...

    // DEBUG
    for (unsigned int i = 0; i < customKeyMaps.length; i++)
    {
        Serial.println(" Before deleting{");
        for (unsigned int j = 0; j < normalKeyCount; j++)
        {
            Serial.print("    .pin: ");
            Serial.println((*customKeyMaps[i])[j].pin);
            Serial.print("    .keyCode: ");
            Serial.println((*customKeyMaps[i])[j].keyCode);
        }
        Serial.println("}");
    }
    delay(100);
    // DEBUG

    Key **removedKeyMapPtr = new Key *;
    bool success = customKeyMaps.RemoveAtIndex(customKeyMapIndex, removedKeyMapPtr);
    // If we successfully removed the keymap...
    if (success)
    {
        Key **nextKeyMapPtr = nullptr;

        // If we deleted the last object in the list...
        if (customKeyMaps.IsEmpty())
        {
            Serial.println("Switched to default keymap"); // DEBUG
            ChangeKeyMap(defaultKeyMap);
            customKeyMapIndex = 0;
        }
        else // the list still contains a keymap...
        {
            // If the current keymap index is out of range...
            if (customKeyMapIndex >= customKeyMaps.length)
            {
                customKeyMapIndex = customKeyMaps.length - 1;
            }

            nextKeyMapPtr = customKeyMaps[customKeyMapIndex];
            if (nextKeyMapPtr != nullptr)
            {
                Serial.print("Switched to keymap "); // DEBUG
                Serial.println(customKeyMapIndex);   // DEBUG
                ChangeKeyMap(*nextKeyMapPtr);
            }
            else
            {
                Serial.print("Failed to delete keymap at "); // DEBUG
                Serial.println(customKeyMapIndex);           // DEBUG
                // TODO: Throw error. We failed to retrieve the keymap at position customKeyMapIndex.
            }
        }

        if (*removedKeyMapPtr != nullptr) // TODO: Is this needed?
        {
            Serial.println("Deleted pointer... *removedKeyMapPtr"); // DEBUG
            delete (*removedKeyMapPtr);
        }
    }
    else
    {
        // TODO: Throw error. We failed to delete the keyMap.
        Serial.print("Something went really wrong..."); // DEBUG
        Serial.println(customKeyMapIndex);              // DEBUG
    }

    delete (removedKeyMapPtr);
    Serial.println("Deleted pointer... removedKeyMapPtr"); // DEBUG
    ToggleEditMode();

    // DEBUG
    Serial.print("Amount of keymaps left: ");
    Serial.println(customKeyMaps.length);
    Serial.print("Current position: ");
    Serial.println(customKeyMapIndex);
    // DEBUG

    // DEBUG
    Serial.println(" After deleting {");
    for (unsigned int i = 0; i < customKeyMaps.length; i++)
    {
        for (unsigned int j = 0; j < normalKeyCount; j++)
        {
            Serial.print("    .pin: ");
            Serial.println((*customKeyMaps[i])[j].pin);
            Serial.print("    .keyCode: ");
            Serial.println((*customKeyMaps[i])[j].keyCode);
        }
        Serial.println("}");
    }
    if (customKeyMaps.length == 0)
    {
        Serial.println("avaiableKeyMaps is Empty");
    }
    delay(100);
    // DEBUG
}

void ToggleEditMode()
{
    bool enteringEditMode = !editmode;

    // If we are trying to start editing the default keymap...
    if (currentKeyMap == defaultKeyMap && enteringEditMode)
    {
        // If we are trying to go into
        // editmode when we have no keymaps...
        if (customKeyMaps.IsEmpty())
        {
            bool success = CreateNewKeyMap();
            if (!success)
            {
                // Error we failed to create the keymap...
                return;
            }
        }
        else
        {
            // We don't want to edit the default keyMap...
            return;
        }
    }

    editmode = !editmode;

    if (editmode && customKeyMaps.length > 0) // If we enter editmode...
    {
        CopyCurrentKeyMapToTemporary();
    }

    ResetEditMode();
}

void ResetEditMode()
{
    // Reset editmode
    editmodeSelectedKey = nullptr;
    editmodeKeysPressed = 0;
    editmodeKeyCode = 0;
    editmodeShouldAddValue = false;

    // Reset led signal
    editmodeNextBlinkCycle = 0;
    editmodeNextBlinkCycleOff = 0;
    editmodeCurrentBlink = 0;
    digitalWrite(LED_BUILTIN, LOW);
    editmodeLedIsOn = false;
}

void CopyCurrentKeyMapToTemporary()
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        editmodeTempCopy[i] = currentKeyMap[i];
    }
}

void ResetCurrentKeyMapToTemporaryCopy()
{
    if (currentKeyMap == defaultKeyMap)
        return;

    // DEBUG
    Serial.println();
    Serial.println("Applying temp to current keymap...");
    for (int i = 0; i < normalKeyCount; i++)
    {
        Serial.print("Temp .pin ");
        Serial.print(editmodeTempCopy[i].pin);
        Serial.print(", .keyCode ");
        Serial.print(editmodeTempCopy[i].keyCode);
        Serial.print(" -> ");

        Serial.print("Current .pin ");
        Serial.print(currentKeyMap[i].pin);
        Serial.print(", .keyCode ");
        Serial.print(currentKeyMap[i].keyCode);
        Serial.println(".");
    }
    delay(100);
    // DEBUG

    for (int i = 0; i < normalKeyCount; i++)
    {
        currentKeyMap[i] = editmodeTempCopy[i];
    }
    ResetEditMode();

    // DEBUG
    Serial.println("Current keymap reset to:");
    for (int i = 0; i < normalKeyCount; i++)
    {
        Serial.print("Current .pin = ");
        Serial.print(currentKeyMap[i].pin);
        Serial.print(", .keyCode = ");
        Serial.println(currentKeyMap[i].keyCode);
    }
    delay(100);
    // DEBUG
}

bool CreateNewKeyMap()
{
    bool successful = false;
    // TODO: Implement real check to see if the arduino can
    // fit another keymap to stack/heap/memory.
    bool weHaveSpaceLeft = customKeyMaps.length < 10;

    if (weHaveSpaceLeft)
    {
        Key *newKeyMap = new Key[normalKeyCount]; // TODO: Maybe remove "new"?
        // Copy the default values to the new keyMap.
        for (int i = 0; i < normalKeyCount; i++)
        {
            newKeyMap[i] = defaultKeyMap[i];
            newKeyMap[i].keyCode = 4; // "a" key.
        }

        // Add it to the list and set it to the current keymap.
        customKeyMaps.Add(newKeyMap);
        int indexOfNewKeyMap = customKeyMaps.length - 1;
        Key **lastKeyMapPtr = customKeyMaps[indexOfNewKeyMap];
        if (lastKeyMapPtr != nullptr)
        {
            ChangeKeyMap(*lastKeyMapPtr);
            customKeyMapIndex = indexOfNewKeyMap;

            successful = true;

            // // DEBUG
            // Serial.println();
            // Serial.println("New current keymap:");
            // for (int i = 0; i < normalKeyCount; i++)
            // {
            //     Serial.print("Current .pin = ");
            //     Serial.print(currentKeyMap[i].pin);
            //     Serial.print(", .keyCode = ");
            //     Serial.println(currentKeyMap[i].keyCode);
            // }
            // Serial.print("Amount of keymaps: ");
            // Serial.println(customKeyMaps.length);
            // delay(100);
            // // DEBUG
        }
        else
        {
            // Serial.println("Something messed up"); // DEBUG
            // TODO: Error we failed to retrieve the newly added keymap.
        }
    }
    else
    {
        // Serial.println("We don't have enought space to create another keymap..."); // DEBUG
        // TODO: Error we don't have space to create another keyMap.
    }

    return successful;
}

void EditMode() // TODO: Check if working... Changed to OnKeyPress and refactored. Might not work? Create tests for this.
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        Key &key = currentKeyMap[i];

        if (OnKeyPress(key))
        {
            digitalWrite(LED_BUILTIN, HIGH);
            editmodeKeysPressed += 1;

            Serial.println("Keypress."); // DEBUG
            Serial.print("Keys pressed: "); // DEBUG
            Serial.println(editmodeKeysPressed); // DEBUG

            if (editmodeSelectedKey == nullptr)
            {
                editmodeSelectedKey = &key;

                Serial.print("Selected key: "); // DEBUG
                Serial.println(editmodeSelectedKey->pin); // DEBUG
            }

            if (!editmodeShouldAddValue && editmodeKeysPressed > 1)
            {
                // If two or more keys are held down we should add a value...
                editmodeShouldAddValue = true;
            }
        }
        else if (OnKeyRelease(key))
        {
            Serial.println("Keyrelease."); // DEBUG
            digitalWrite(LED_BUILTIN, LOW);

            if (editmodeSelectedKey != nullptr)
            {
                if (editmodeShouldAddValue)
                {
                    // Raise value of keycode.
                    int exponent = editmodeKeysPressed - 2;
                    int numberToAdd = pow(10, exponent);
                    editmodeKeyCode += numberToAdd;

                    editmodeShouldAddValue = false;
                    
                    // DEBUG
                    Serial.print("Inputed keycode: ");
                    Serial.print(editmodeKeyCode);
                    Serial.print(", (for pin: ");
                    Serial.print(editmodeSelectedKey->pin);
                    Serial.print(", keycode: ");
                    Serial.print(editmodeSelectedKey->keyCode);
                    Serial.println(")");
                    // DEBUG
                }
            } 
            else 
                Serial.println("Selected is nullptr!"); // DEBUG

            editmodeKeysPressed -= 1;

            // If we are releasing the last pressed key...
            if (editmodeKeysPressed <= 0)
            {
                editmodeSelectedKey->keyCode = editmodeKeyCode;

                // DEBUG
                Serial.print("Updated key: .pin = ");
                Serial.print(editmodeSelectedKey->pin);
                Serial.print(", .keyCode = ");
                Serial.println(editmodeSelectedKey->keyCode);
                // DEBUG

                ResetEditMode();
            }

            Serial.print("Amount of keys pressed: "); // DEBUG
            Serial.println(editmodeKeysPressed); // DEBUG
        }
    }

    // Signal that we are in edit mode.
    if (editmodeKeysPressed == 0)
    {
        SignalLedEditMode();
    }
}

/**
 * @brief When this function is called inside the loop it will
 * produce an "editmode" signal by flashing the LED 13.
 */
void SignalLedEditMode()
{
    unsigned long currentTime = millis();

    // if its time to turn off led...
    if (editmodeLedIsOn)
    {
        if (editmodeNextBlinkCycleOff < currentTime)
        {
            digitalWrite(LED_BUILTIN, LOW);
            editmodeLedIsOn = false;
            editmodeNextBlinkCycle = currentTime + 100;
        }
    }
    else
    {
        // If its time to blink...
        if (editmodeNextBlinkCycle < currentTime)
        {
            // If we should pulse...
            if (editmodeCurrentBlink < editmodeBlinksPerSignal)
            {
                digitalWrite(LED_BUILTIN, HIGH);
                editmodeLedIsOn = true;

                editmodeCurrentBlink++;
                editmodeNextBlinkCycle = currentTime + 200;
            }
            else // we are done pulsing...
            {
                editmodeCurrentBlink = 0;
                editmodeNextBlinkCycle = currentTime + 2000;
            }

            editmodeNextBlinkCycleOff = currentTime + 200;
        }
    }
}

/**
 * @brief Reads and updates the value of a
 * keys pin with the debounced input.
 * 
 * @param key The key to be updated.
 */
void DebounceRead(IPinState &key) // TODO: This causes a slight input delay. Consider this: if you were to press the button every <30ms the input would not be registered.
{
    key.oldValue = key.value;
    unsigned int debounceDelay = 30; // TODO: This balance needs to be play tested.
    unsigned long currentTime = millis();

    // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    bool pinState = !digitalRead(key.pin);

    // If the pin state has changed...
    if (pinState != key.oldPinState)
    {
        key.lastDebounceTime = currentTime;
        // // Print debounce catches.
        // Serial.print("he");
    }

    unsigned long timePassedSinceDebounce = (currentTime - key.lastDebounceTime);
    // If we've waited long enough since last debounce...
    if (timePassedSinceDebounce > debounceDelay)
    {
        // And if the old state is not already the new state...
        if (pinState != key.value)
        {
            key.value = pinState;

            if (key.value)
            {
                key.timeOfActivation = currentTime;
            }

            // // Print debounce catches.
            // if(key.value) {
            //     Serial.print(" hej");
            // } else {Serial.print(" hå");}
            // Serial.println();
        }
    }

    key.oldPinState = pinState;
}

bool OnKeyPress(IPinState &key)
{
    return (key.oldValue != key.value && key.value);
}

bool OnKeyRelease(IPinState &key)
{
    return (key.oldValue != key.value && !key.value);
}

void SignalErrorToUser()
{
    // We can't cycle through 0 keymaps...
    // Signal that something is wrong.
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
    }
    return;
}

// Miscellaneous

// Key setup WASD
// Key keys[4] = {
// {.pin = 2, .keyCode = 4},
// {.pin = 3, .keyCode = 26},
// {.pin = 4, .keyCode = 22},
// {.pin = 5, .keyCode = 7},
// };

// Key setup Arrow keys
// Key keys[4] = {
//     {.pin = 2, .keyCode = 80},
//     {.pin = 3, .keyCode = 82},
//     {.pin = 4, .keyCode = 81},
//     {.pin = 5, .keyCode = 79},
// };