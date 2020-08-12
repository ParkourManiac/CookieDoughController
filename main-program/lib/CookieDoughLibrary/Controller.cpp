#include "Controller.h"

#include <EEPROM.h>
#include <Arduino.h>

void Controller::Setup()
{
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

    LoadKeyMapsFromMemory(customKeyMaps);
    ConfigurePinsForKeyMap<Key>(currentKeyMap, normalKeyCount);
    ConfigurePinsForKeyMap<SpecialKey>(specialKeys, specialKeyCount);

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

void Controller::Update()
{
    ReadPinValuesForKeyMap(currentKeyMap, normalKeyCount);
    ReadPinValuesForKeyMap(specialKeys, specialKeyCount);

    ExecuteSpecialCommands();
    if (editmode.enabled)
    {
        editmode.EditModeLoop(currentKeyMap);
    }
    else
    {
        SendKeyInfo();
    }
}

void Controller::SaveKeyMapsToMemory(LinkedList<Key *> keymapList) // TODO: Needs to be tested.
{
    unsigned int serializedSize = sizeof(BareKeyboardKey[keymapList.length * normalKeyCount]);
    // Key *serializedKeyMaps = new Key[keymapList.length * normalKeyCount];
    BareKeyboardKey *serializedKeyMaps = new BareKeyboardKey[keymapList.length * normalKeyCount];
    for (unsigned int i = 0; i < keymapList.length; i++)
    {
        for (int j = 0; j < normalKeyCount; j++)
        {
            unsigned int pos = i * normalKeyCount + j;
            serializedKeyMaps[pos] = (BareKeyboardKey)(*keymapList[i])[j];
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

void Controller::LoadKeyMapsFromMemory(LinkedList<Key *> &keymapList)
{
    unsigned int packetAdress = 0;
    unsigned int packetSize;
    DataPacket *dataPtr = new DataPacket();
    DataPacket packet = *dataPtr;

    bool foundPacket = RetrieveDataPacketFromMemory(packet, packetSize, packetAdress);
    if (!foundPacket) return;

    Serial.println("Began Converting DataPacket to Keymaps..."); // DEBUG
    delay(100); // DEBUG

    unsigned int amountOfKeys = packet.payloadLength / sizeof(BareKeyboardKey);
    BareKeyboardKey payloadAsBareKeys[amountOfKeys];
    ConvertDataPacketToBareKeyboardKeys(packet, payloadAsBareKeys);

    // TODO: Check if the bare keys are somewhat reasonable
    // TODO: Check if the pin numbers are present in the default keymap.
    for (int i = 0; i < amountOfKeys; i++)
    {
        bool isValid = IsKeyValid(payloadAsBareKeys[i]);
        if (!isValid) return;
    }

    LoadBareKeyboardKeysIntoKeymapList(payloadAsBareKeys, amountOfKeys, keymapList);

    // DEBUG
    Serial.println("Data:");
    for (unsigned int i = 0; i < keymapList.length; i++)
    {
        Serial.println("{");
        for (int j = 0; j < normalKeyCount; j++)
        {
            Serial.print("    .pin: ");
            Serial.println((*keymapList[i])[j].pin);
            Serial.print("    .keyCode: ");
            Serial.println((*keymapList[i])[j].keyCode);
        }
        Serial.println("}");
    }
    delay(100);
    // DEBUG

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
    delete(dataPtr);
}

bool Controller::RetrieveDataPacketFromMemory(DataPacket &packet, unsigned int &packetSize, unsigned int &packetAdress)
{
    packetAdress = 0;
    packetSize = 0;
    bool foundPacket = false;
    do
    {
        foundPacket = ParsePacketFromEEPROM(packetAdress, packet, packetSize);
        if (!foundPacket) {
            packetAdress++;
            if (packetAdress >= EEPROM.length())
            {
                Serial.println("Failed to read data from memory!"); // DEBUG
                delay(100); // DEBUG

                return false;
            }
        }
    } while (!foundPacket && packetAdress < EEPROM.length());

    return foundPacket;
}

void Controller::ConvertDataPacketToBareKeyboardKeys(DataPacket packet, BareKeyboardKey *result)
{
    // Convert to bare keys
    unsigned int amountOfKeys = packet.payloadLength / sizeof(BareKeyboardKey);
    for (unsigned int i = 0; i < amountOfKeys; i++)
    {
        result[i] = ((BareKeyboardKey *)packet.payload)[i];
    }
}

void Controller::LoadBareKeyboardKeysIntoKeymapList(BareKeyboardKey *keys, unsigned int amountOfKeys, LinkedList<Key *> &keymapList) {
    // Convert bare keys to keys with pin state
    unsigned int amountOfKeymaps = amountOfKeys / normalKeyCount;
    for (unsigned int i = 0; i < amountOfKeymaps; i++) // For each keymap
    {
        Key *keyMap = new Key[normalKeyCount];
        for (int j = 0; j < normalKeyCount; j++) // For each key in a keymap
        {
            BareKeyboardKey currentKey = keys[i * normalKeyCount + j];
            keyMap[j].pin = currentKey.pin;
            keyMap[j].keyCode = currentKey.keyCode;

            // DEBUG
            Serial.println("BareKey:");
            Serial.print("    .pin: ");
            Serial.println(currentKey.pin);
            Serial.print("    .keyCode: ");
            Serial.println(currentKey.keyCode);

            Serial.println("ConvertedKey:");
            Serial.print("    .pin: ");
            Serial.println(keyMap[j].pin);
            Serial.print("    .keyCode: ");
            Serial.println(keyMap[j].keyCode);
            delay(100);
            // DEBUG
        }
        keymapList.Add(keyMap);
    }
}

bool Controller::IsKeyValid(IKey key)
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        if (key.pin == defaultKeyMap[i].pin)
            return true;
    }

    return false;
}

void Controller::CycleKeyMap() // TODO: Check if working.
{
    if (customKeyMaps.IsEmpty())
    {
        // We can't cycle through an empty list of keymaps...
        Serial.println("Empty"); // DEBUG
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

void Controller::ChangeKeyMap(Key *keyMap)
{
    Serial.println("Changing keymap"); // DEBUG
    currentKeyMap = keyMap;
    ConfigurePinsForKeyMap(currentKeyMap, normalKeyCount);
}

void Controller::ToggleDefaultKeyMap()
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

void Controller::SendKeyInfo()
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
        else if (OnKeyRelease(key))
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

void Controller::ExecuteSpecialCommands()
{
    for (int i = 0; i < specialKeyCount; i++)
    {
        SpecialKey &specialKey = specialKeys[i];

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
                    if (editmode.enabled)
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
                    if (editmode.enabled)
                    {
                        if (currentKeyMap != defaultKeyMap)
                        {
                            editmode.RestoreKeyMapFromTemporaryCopy(currentKeyMap);
                        }
                        else
                        {
                            SignalErrorToUser();
                        }
                    }
                    else
                    {
                        ToggleDefaultKeyMap();
                    }
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
                    if (editmode.enabled)
                    {
                        wasALongPress = OnLongPress(specialKey, longPressDuration);
                        // if we did a long press in editmode....
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
                    if (editmode.enabled)
                    {
                        if (OnLongPress(specialKey, longPressDuration))
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

void Controller::ToggleEditMode() {
    bool enteringEditMode = !editmode.enabled;

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

    editmode.Toggle();

    if (editmode.enabled && customKeyMaps.length > 0) // If we enter ...
    {
        editmode.CopyKeyMapToTemporary(currentKeyMap);
    }
}

void Controller::SaveControllerSettings()
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

void Controller::DeleteCurrentKeyMap()
{
    if (!editmode.enabled)
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
        for (int j = 0; j < normalKeyCount; j++)
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

    // TODO: Try replacing **removedKeyMapPtr with *removedKeyMapPtr and pass in &removedKeyMapPtr to the function RemoveAtIndex.
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
        for (int j = 0; j < normalKeyCount; j++)
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

bool Controller::CreateNewKeyMap()
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

void Controller::SignalErrorToUser()
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