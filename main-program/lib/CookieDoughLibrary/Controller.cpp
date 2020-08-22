#include "Controller.h"

#include <EEPROM.h>
#include <Arduino.h>
#include <stdlib.h>

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
    // customKeyMaps.Add(keys); // NOTE: HAS BEEN REFACTORED TO BARE KEYBOARD KEYS. PLEASE CHANGE THIS.
    // SaveKeyMapsToMemory(customKeyMaps); // NOTE: HAS BEEN REFACTORED TO BARE KEYBOARD KEYS.
    ChangeKeyMap(defaultKeyMap);
    LoadKeymapsFromMemoryIntoList(customKeyMaps); // SRAM: -162 (When loading one keymap of 4 keys).
    ConfigurePinsForKeyMap<Key>(currentKeyMap, normalKeyCount); //SRAM: -0
    ConfigurePinsForKeyMap<SpecialKey>(specialKeys, specialKeyCount); //SRAM: -0

    // // DEBUG
    // DEBUG_PRINTLN();
    // DEBUG_PRINTLN("New current keymap:");
    // for(int i = 0; i < normalKeyCount; i++) {
    //     DEBUG_PRINT("Current .pin = ");
    //     DEBUG_PRINT(currentKeyMap[i].pin);
    //     DEBUG_PRINT(", .keyCode = ");
    //     DEBUG_PRINTLN(currentKeyMap[i].keyCode);
    // }
    // delay(100);

    // for (unsigned int i = 0; i < customKeyMaps.length; i++)
    // {
    //     DEBUG_PRINTLN("{");
    //     for (unsigned int j = 0; j < normalKeyCount; j++)
    //     {
    //         DEBUG_PRINT("    .pin: ");
    //         DEBUG_PRINTLN((*customKeyMaps[i])[j].pin);
    //         DEBUG_PRINT("    .keyCode: ");
    //         DEBUG_PRINTLN((*customKeyMaps[i])[j].keyCode);
    //     }
    //     DEBUG_PRINTLN("}");
    // }
    // // DEBUG
}

void Controller::Update()
{
    ReadPinValuesForKeyMap(currentKeyMap, normalKeyCount); // SRAM: -6
    ReadPinValuesForKeyMap(specialKeys, specialKeyCount); // SRAM: -6

    ExecuteSpecialCommands(); // SRAM: -105
    if (editmode.enabled)
    {
        editmode.EditModeLoop(currentKeyMap); // SRAM: -2
    }
    else
    {
        SendKeyInfo(); // SRAM: -2
    }
}

void Controller::SaveKeyMapsToMemory(LinkedList<BareKeyboardKey *> keymapList) // TODO: Refactored to BareKeyboardKey. Check if code is necessary.
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
    // DEBUG_PRINT("Passed in: ");
    // for(int i = 0; i < serializedSize; i++) {
    //     DEBUG_PRINT(dataPtr[i], HEX);
    // }
    // DEBUG_PRINTLN();
    // delay(100);
    // // DEBUG
    unsigned int packetSize;
    // TODO: Change this to write to nextFreeEepromAdress and invalidate the old one at eepromAdress.
    bool success = SavePacketToEEPROM(eepromAdress, dataPtr, serializedSize, packetSize);
    if (!success)
    {
        DEBUG_PRINTLN("Failed to write data to memory!"); // DEBUG
        delay(100); // DEBUG

        // TODO: Implement error code.
    } else {
        DEBUG_PRINTLN("Settings saved!"); // DEBUG
        delay(100); // DEBUG
    }
    nextFreeEepromAdress = eepromAdress + packetSize;

    delay(500);
    delete (serializedKeyMaps);
}

void Controller::LoadKeymapsFromMemoryIntoList(LinkedList<BareKeyboardKey *> &keymapList) // Refactored to BareKeyboardKey.
{
    unsigned int packetAdress;
    unsigned int packetSize;

    unsigned int amountOfKeys;
    BareKeyboardKey *payloadAsBareKeys = new BareKeyboardKey[1];
    bool success = RetrieveBareKeyboardKeysFromMemory(payloadAsBareKeys, amountOfKeys, packetAdress, packetSize);
    if (!success) 
    {
        delete[](payloadAsBareKeys);
        return;
    }

    ParseBareKeyboardKeyArrayIntoKeymapList(payloadAsBareKeys, amountOfKeys, keymapList);
    delete[](payloadAsBareKeys);

    // DEBUG
    for (unsigned int i = 0; i < keymapList.length; i++)
    {
        DEBUG_PRINT("Data ");
        DEBUG_PRINT(i);
        DEBUG_PRINTLN(":");
        for (int j = 0; j < normalKeyCount; j++)
        {
            DEBUG_PRINT("    ( pin: ");
            DEBUG_PRINT((*keymapList[i])[j].pin);
            DEBUG_PRINT(", keyCode: ");
            DEBUG_PRINT((*keymapList[i])[j].keyCode);
            DEBUG_PRINTLN(" )");
        }
    }
    delay(100);
    // DEBUG

    // DEBUG
    // DEBUG_PRINTLN("DATA:::::");
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     DEBUG_PRINTLN(packet.payload[i], HEX);
    // }
    // DEBUG_PRINTLN(":::::");

    // DEBUG_PRINTLN("Finished loading.");

    // DEBUG_PRINTLN();
    // DEBUG_PRINT("Packet size: ");
    // DEBUG_PRINTLN(packetSize);
    // delay(100);
    // DEBUG

    eepromAdress = packetAdress;
    nextFreeEepromAdress = packetAdress + packetSize;
}

bool Controller::RetrieveBareKeyboardKeysFromMemory(BareKeyboardKey *&payloadAsBareKeys, unsigned int &amountOfKeys, unsigned int &packetAdress, unsigned int &packetSize)
{
    amountOfKeys = packetAdress = packetSize = 0;
    DataPacket *dataPtr = new DataPacket();
    DataPacket packet = *dataPtr;

    bool foundValidPacket = false;
    while (!foundValidPacket)
    {
        unsigned int startAdress = packetAdress;
        bool foundPacket = RetrieveDataPacketFromMemory(packet, packetSize, packetAdress, startAdress);
        if (!foundPacket)
            return false;

        // DEBUG_PRINTLN("Began Converting DataPacket to Keymaps..."); // DEBUG
        // delay(100);                                                  // DEBUG

        amountOfKeys = packet.payloadLength / sizeof(BareKeyboardKey);
        delete[](payloadAsBareKeys);
        payloadAsBareKeys = new BareKeyboardKey[amountOfKeys];
        ConvertDataPacketToBareKeyboardKeys(packet, payloadAsBareKeys);

        foundValidPacket = true;
        for (unsigned int i = 0; i < amountOfKeys; i++)
        {
            bool isValid = IsKeyValid(payloadAsBareKeys[i]);

            // DEBUG
            DEBUG_PRINT("IsValid?: "); 
            DEBUG_PRINT(isValid); 
            DEBUG_PRINT("  {");
            DEBUG_PRINT(" .pin: ");
            DEBUG_PRINT(payloadAsBareKeys[i].pin);
            DEBUG_PRINT(", .keyCode: ");
            DEBUG_PRINT(payloadAsBareKeys[i].keyCode);
            DEBUG_PRINTLN(" }");
            delay(100);
            // DEBUG
            if (!isValid)
            {
                foundValidPacket = false;
                packetAdress += packetSize;
                break;
            }
        }
    }

    delete (dataPtr);
    return true;
}

bool Controller::RetrieveDataPacketFromMemory(DataPacket &packet, unsigned int &packetSize, unsigned int &packetAdress, unsigned int startAdress)
{
    packetAdress = startAdress;
    packetSize = 0;
    bool foundPacket = false;
    do
    {
        foundPacket = ParsePacketFromEEPROM(packetAdress, packet, packetSize);
        if (!foundPacket)
        {
            packetAdress++;
            if (packetAdress >= EEPROM.length())
            {
                DEBUG_PRINTLN("Failed to read data from memory!"); // DEBUG
                delay(100);                                         // DEBUG

                return false;
            }
        }
    } while (!foundPacket && packetAdress < EEPROM.length());

    return foundPacket;
}

void Controller::ConvertDataPacketToBareKeyboardKeys(DataPacket packet, BareKeyboardKey *result)
{
    unsigned int amountOfKeys = packet.payloadLength / sizeof(BareKeyboardKey);
    for (unsigned int i = 0; i < amountOfKeys; i++)
    {
        result[i] = ((BareKeyboardKey *)packet.payload)[i];
    }
}

void Controller::ParseBareKeyboardKeyArrayIntoKeymapList(BareKeyboardKey *keys, unsigned int amountOfKeys, LinkedList<BareKeyboardKey *> &keymapList) // NOTE: Refactored to BareKeyboardKeys.
{
    // Convert bare keys to keys with pin state
    unsigned int amountOfKeymaps = amountOfKeys / normalKeyCount;
    for (unsigned int i = 0; i < amountOfKeymaps; i++) // For each keymap
    {
        BareKeyboardKey *keyMap = new BareKeyboardKey[normalKeyCount];
        for (int j = 0; j < normalKeyCount; j++) // For each key in a keymap
        {
            keyMap[j] = keys[i * normalKeyCount + j];

            // // DEBUG
            // DEBUG_PRINTLN("BareKey:");
            // DEBUG_PRINT("    .pin: ");
            // DEBUG_PRINTLN(currentKey.pin);
            // DEBUG_PRINT("    .keyCode: ");
            // DEBUG_PRINTLN(currentKey.keyCode);

            // DEBUG_PRINTLN("ConvertedKey:");
            // DEBUG_PRINT("    .pin: ");
            // DEBUG_PRINTLN(keyMap[j].pin);
            // DEBUG_PRINT("    .keyCode: ");
            // DEBUG_PRINTLN(keyMap[j].keyCode);
            // delay(100);
            // // DEBUG
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

void Controller::CycleKeyMap() // TODO: Check if working. // Refactored for BareKeyboardKeys.
{
    if (customKeyMaps.IsEmpty())
    {
        // We can't cycle through an empty list of keymaps...
        DEBUG_PRINTLN("Empty"); // DEBUG
        SignalErrorToUser();
        return;
    }

    // If we are using the default. Switch back to the
    // previous keymap. Otherwise move to the next.
    int nextIndex = (isUsingDefaultKeymap) ? customKeyMapIndex : customKeyMapIndex + 1;

    customKeyMapIndex = nextIndex % customKeyMaps.length;
    BareKeyboardKey **nextKeyMapPtr = customKeyMaps[customKeyMapIndex];

    if (nextKeyMapPtr != nullptr)
    {
        BareKeyboardKey *nextKeyMap = *(nextKeyMapPtr);
        ChangeKeyMap(nextKeyMap);
    }
}

void Controller::ChangeKeyMap(BareKeyboardKey *keyMap) // Refactored for BareKeyboardKeys. // TODO: Needs to be tested.
{
    DEBUG_PRINTLN("Changing keymap"); // DEBUG
    // Overwrite currentKeyMap with the keys we want to equip.
    for(int i = 0; i < normalKeyCount; i++) 
    {
        currentKeyMap[i] = Key(keyMap[i].pin, keyMap[i].keyCode);
    }

    // Perform a key release for all active buttons.
    WipeKeyboardEventBuffer();
    SendKeyboardEvent();

    // Keep track if we are using the default keymap.
    isUsingDefaultKeymap = keyMap == defaultKeyMap; // NOTE: This line is already tested.

    // Configure pins
    ConfigurePinsForKeyMap(currentKeyMap, normalKeyCount);
}

void Controller::ToggleDefaultKeyMap() // NOTE: Refactored to BareKeyboardKeys // TODO: Needs to be tested.
{
    bool toggleToDefault = !isUsingDefaultKeymap;
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
        BareKeyboardKey **lastKeyMapPtr = customKeyMaps[customKeyMapIndex];
        if (lastKeyMapPtr != nullptr)
        {
            ChangeKeyMap(*lastKeyMapPtr);
        }
    }
}

void Controller::SendKeyInfo() // TODO: Needs to be tested
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
                SendKeyboardEvent();
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
                SendKeyboardEvent();
            }
        }
    }
}

void Controller::WipeKeyboardEventBuffer() 
{
    for(int i = 0; i < 8; i++) 
    {
        buf[i] = 0;
    }
}

void Controller::SendKeyboardEvent() 
{
    Serial.write(buf, 8);
}

void Controller::ExecuteSpecialCommands() // TODO: Needs to be tested.
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
                        if (!isUsingDefaultKeymap)
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
                    if (editmode.enabled)
                    {
                        // if we did a long press in editmode....
                        if (OnLongPress(specialKey, longPressDuration))
                        {
                            // DEBUG_PRINTLN("Long press, released. Save to memory..."); // DEBUG
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
                            // DEBUG_PRINTLN("Long press, released. Delete keymap..."); // DEBUG
                            DeleteCurrentKeyMap();
                        }
                    }
                    break;
                }
                }

                //DEBUG_PRINTLN((millis() - specialKey.timeOfActivation)); // DEBUG
            }
        }
    }
}

void Controller::ToggleEditMode() // TODO: Needs to be tested.
{
    bool enteringEditMode = !editmode.enabled;

    // If we are trying to start editing the default keymap...
    if (isUsingDefaultKeymap && enteringEditMode)
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
            SignalErrorToUser();
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

void Controller::SaveControllerSettings() // TODO: Needs to be tested.
{
    SaveKeyMapsToMemory(customKeyMaps);

    unsigned long timeNeeded = customKeyMaps.length * normalKeyCount * sizeof(BareKeyboardKey) * 5;

    // // DEBUG
    // DEBUG_PRINT("Time needed to save: ");
    // DEBUG_PRINTLN(timeNeeded);
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

void Controller::DeleteCurrentKeyMap() // NOTE: Refactored to BareKeyboardKeys. // TODO: Needs to be tested.
{
    if (!editmode.enabled)
        return;
    if (customKeyMaps.IsEmpty())
        return;
    if (isUsingDefaultKeymap)
        return;
    // If we did not return above:
    // We are in editmode, we have atleast one keymap
    // in our list and we are not trying to
    // delete the default keymap...

    // DEBUG
    for (unsigned int i = 0; i < customKeyMaps.length; i++)
    {
        DEBUG_PRINT("Before deleting ");
        DEBUG_PRINT(i);
        DEBUG_PRINTLN(":");
        for (int j = 0; j < normalKeyCount; j++)
        {
            DEBUG_PRINT("    ( pin: ");
            DEBUG_PRINT((*customKeyMaps[i])[j].pin);
            DEBUG_PRINT(", keyCode: ");
            DEBUG_PRINT((*customKeyMaps[i])[j].keyCode);
            DEBUG_PRINTLN(" )");
        }
    }
    delay(100);
    // DEBUG

    // TODO: Try replacing **removedKeyMapPtr with *removedKeyMapPtr and pass in &removedKeyMapPtr to the function RemoveAtIndex.
    BareKeyboardKey **removedKeyMapPtr = new BareKeyboardKey *;
    bool success = customKeyMaps.RemoveAtIndex(customKeyMapIndex, removedKeyMapPtr);
    // If we successfully removed the keymap...
    if (success)
    {
        BareKeyboardKey **nextKeyMapPtr = nullptr;

        // If we deleted the last object in the list...
        if (customKeyMaps.IsEmpty())
        {
            DEBUG_PRINTLN("Switched to default keymap"); // DEBUG
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
                DEBUG_PRINT("Switched to keymap "); // DEBUG
                DEBUG_PRINTLN(customKeyMapIndex);   // DEBUG
                ChangeKeyMap(*nextKeyMapPtr);
            }
            else
            {
                DEBUG_PRINT("Failed to delete keymap at "); // DEBUG
                DEBUG_PRINTLN(customKeyMapIndex);           // DEBUG
                // TODO: Throw error. We failed to retrieve the keymap at position customKeyMapIndex.
            }
        }

        if (*removedKeyMapPtr != nullptr) // TODO: Is this needed?
        {
            DEBUG_PRINTLN("Deleted pointer... *removedKeyMapPtr"); // DEBUG
            delete (*removedKeyMapPtr);
        }
    }
    else
    {
        // TODO: Throw error. We failed to delete the keyMap.
        DEBUG_PRINT("Something went really wrong..."); // DEBUG
        DEBUG_PRINTLN(customKeyMapIndex);              // DEBUG
    }

    delete (removedKeyMapPtr);                             // TODO: Check if this is correct or not.
    DEBUG_PRINTLN("Deleted pointer... removedKeyMapPtr"); // DEBUG
    ToggleEditMode();

    // DEBUG
    DEBUG_PRINT("Amount of keymaps left: ");
    DEBUG_PRINTLN(customKeyMaps.length);
    DEBUG_PRINT("Current position: ");
    DEBUG_PRINTLN(customKeyMapIndex);
    // DEBUG

    // DEBUG
    for (unsigned int i = 0; i < customKeyMaps.length; i++)
    {
        DEBUG_PRINT("After deleting ");
        DEBUG_PRINT(i);
        DEBUG_PRINTLN(":");
        for (int j = 0; j < normalKeyCount; j++)
        {
            DEBUG_PRINT("    ( pin: ");
            DEBUG_PRINT((*customKeyMaps[i])[j].pin);
            DEBUG_PRINT(", keyCode: ");
            DEBUG_PRINT((*customKeyMaps[i])[j].keyCode);
            DEBUG_PRINTLN(" )");
        }
    }
    if (customKeyMaps.length == 0)
    {
        DEBUG_PRINTLN("avaiableKeyMaps is Empty");
    }
    delay(100);
    // DEBUG
}

bool Controller::CreateNewKeyMap() // TODO: Needs to be tested.
{
    bool successful = false;
    // TODO: Implement real check to see if the arduino can
    // fit another keymap to stack/heap/memory.
    bool weHaveSpaceLeft = customKeyMaps.length < 10;

    if (weHaveSpaceLeft)
    {
        BareKeyboardKey *newKeyMap = new BareKeyboardKey[normalKeyCount]; // TODO: Maybe remove "new"? // TODO: POTENTIAL SRAM LEAK????!!!!
        int initialKeycode = 4; // The "a" key.
        // Copy the default pin values to the new keyMap.
        for (int i = 0; i < normalKeyCount; i++)
        {
            newKeyMap[i] = BareKeyboardKey(defaultKeyMap[i].pin, initialKeycode);
        }

        // Add it to the list and set it to the current keymap.
        customKeyMaps.Add(newKeyMap);
        int indexOfNewKeyMap = customKeyMaps.length - 1;
        BareKeyboardKey **lastKeyMapPtr = customKeyMaps[indexOfNewKeyMap];
        if (lastKeyMapPtr != nullptr)
        {
            ChangeKeyMap(*lastKeyMapPtr);
            customKeyMapIndex = indexOfNewKeyMap;

            successful = true;

            //DEBUG_PRINTLN("Created new keymap!"); // DEBUG
            //delay(100); // DEBUG

            // // DEBUG
            // DEBUG_PRINTLN();
            // DEBUG_PRINTLN("New current keymap:");
            // for (int i = 0; i < normalKeyCount; i++)
            // {
            //     DEBUG_PRINT("Current .pin = ");
            //     DEBUG_PRINT(currentKeyMap[i].pin);
            //     DEBUG_PRINT(", .keyCode = ");
            //     DEBUG_PRINTLN(currentKeyMap[i].keyCode);
            // }
            // DEBUG_PRINT("Amount of keymaps: ");
            // DEBUG_PRINTLN(customKeyMaps.length);
            // delay(100);
            // // DEBUG
        }
        else
        {
            // DEBUG_PRINTLN("Something messed up"); // DEBUG
            // TODO: Error we failed to retrieve the newly added keymap.
        }
    }
    else
    {
        // DEBUG_PRINTLN("We don't have enought space to create another keymap..."); // DEBUG
        // TODO: Error we don't have space to create another keyMap.
    }

    return successful;
}

void Controller::SignalErrorToUser() // TODO: Needs to be tested?
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