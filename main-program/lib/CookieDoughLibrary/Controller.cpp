#include "Controller.h"

#include <EEPROM.h>
#include <Arduino.h>
#include <stdlib.h>

Controller::Controller(BareKeyboardKey *_defaultKeymap, int amountOfDefaultKeys, SpecialKey *_specialKeys, int amountOfSpecialKeys)
    : normalKeyCount(amountOfDefaultKeys)
    , defaultKeymap(new BareKeyboardKey[this->normalKeyCount])
    , specialKeyCount(amountOfSpecialKeys)
    , specialKeys(new SpecialKey[this->specialKeyCount])
    , currentKeyMap(new Key[this->normalKeyCount])
    , customKeyMapsPtr(new LinkedList<BareKeyboardKey *>())
    , customKeyMaps(*(this->customKeyMapsPtr))
    , buf(new uint8_t[this->bufferSize]{ 0 })
{
    for(int i = 0; i < normalKeyCount; i++) 
    {
        defaultKeymap[i] = _defaultKeymap[i];
    }

    for(int i = 0; i < specialKeyCount; i++) 
    {
        specialKeys[i] = _specialKeys[i];
    }
}

Controller::Controller(const Controller& other) 
    : Controller(other.defaultKeymap, other.normalKeyCount, other.specialKeys, other.specialKeyCount) 
{   
}

Controller::~Controller()
{
    delete[](defaultKeymap);
    delete[](specialKeys);
    delete[](currentKeyMap);
    delete(customKeyMapsPtr);
    delete[](buf);
}

void Controller::Setup()
{
    // // DEBUG CLEAN EEPROM
    // for(unsigned int i = 0; i < EEPROM.length(); i++) {
    //     EEPROM.write(i, 0);
    // }
    // // DEBUG

    DEBUG_PRINT("\nChanging to default keymap.\n");
    DEBUG(delay(100));
    ChangeKeyMap(defaultKeymap);
    LoadKeymapsFromMemoryIntoList(&customKeyMaps); // SRAM: -162 (When loading one keymap of 4 keys).
    ConfigurePinsForKeyMap<Key>(currentKeyMap, normalKeyCount); //SRAM: -0
    ConfigurePinsForKeyMap<SpecialKey>(specialKeys, specialKeyCount); //SRAM: -0
}

void Controller::Update()
{
    UpdatePinStatesForKeyMap(currentKeyMap, normalKeyCount); // SRAM: -6
    UpdatePinStatesForKeyMap(specialKeys, specialKeyCount); // SRAM: -6

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

void Controller::SaveKeyMapsToMemory(LinkedList<BareKeyboardKey *> keymapList)
{
    const int keyCount = keymapList.length * normalKeyCount;
    BareKeyboardKey *serializedKeyMaps = new BareKeyboardKey[keyCount];
    uint16_t serializedSize = static_cast<uint16_t>(sizeof(serializedKeyMaps[0]) * keyCount);
    for (unsigned int i = 0; i < keymapList.length; i++)
    {
        for (int j = 0; j < normalKeyCount; j++)
        {
            unsigned int pos = i * normalKeyCount + j;
            serializedKeyMaps[pos] = (*keymapList[i])[j];
        }
    }

    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(serializedKeyMaps);
    // // DEBUG
    // DEBUG_PRINT("Passed in: ");
    // for(int i = 0; i < serializedSize; i++) {
    //     DEBUG_PRINT(dataPtr[i], HEX);
    // }
    // DEBUG_PRINT("\n");
    // DEBUG(delay(100));
    // // DEBUG
    uint16_t packetSize;
    // TODO: Change this to write to nextFreeEepromAdress and invalidate the old one at eepromAdress.
    bool success = SavePacketToEEPROM(eepromAdress, dataPtr, serializedSize, &packetSize);
    if (success)
    {
        DEBUG_PRINT("Settings saved!\n"); // DEBUG
        DEBUG(delay(100)); // DEBUG
        nextFreeEepromAdress = static_cast<uint16_t>(eepromAdress + packetSize);
    } else 
    {
        DEBUG_PRINT("Failed to write data to memory!\n"); // DEBUG
        DEBUG(delay(100)); // DEBUG

        // TODO: Implement error code.
    }

    delete (serializedKeyMaps);
}

void Controller::LoadKeymapsFromMemoryIntoList(LinkedList<BareKeyboardKey *> *keymapList) // Refactored to BareKeyboardKey.
{
    uint16_t packetAdress, packetSize, amountOfKeys;
    BareKeyboardKey *payloadAsBareKeys = new BareKeyboardKey[1];
    bool success = RetrieveBareKeyboardKeysFromMemory(&payloadAsBareKeys, &amountOfKeys, &packetAdress, &packetSize);
    if (!success) 
    {
        delete[](payloadAsBareKeys);
        return;
    }

    ParseBareKeyboardKeyArrayIntoKeymapList(payloadAsBareKeys, amountOfKeys, keymapList);
    delete[](payloadAsBareKeys);

    DEBUG(
        for (unsigned int i = 0; i < keymapList->length; i++)
        {
            DEBUG_PRINT("Data ");
            DEBUG_PRINT(i);
            DEBUG_PRINT(":\n");
            for (int j = 0; j < normalKeyCount; j++)
            {
                DEBUG_PRINT("    ( pin: ");
                DEBUG_PRINT((*(*keymapList)[i])[j].pin);
                DEBUG_PRINT(", keyCode: ");
                DEBUG_PRINT((*(*keymapList)[i])[j].keyCode);
                DEBUG_PRINT(" )\n");
            }
        }
        DEBUG(delay(100));
    );

    // DEBUG
    // DEBUG_PRINT("DATA:::::\n");
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     DEBUG_PRINT(packet.payload[i], HEX);
    //     DEBUG_PRINT("\n");
    // }
    // DEBUG_PRINT(":::::\n");

    // DEBUG_PRINT("Finished loading.\n");

    // DEBUG_PRINT("\n");
    // DEBUG_PRINT("Packet size: ");
    // DEBUG_PRINT(packetSize);
    // DEBUG_PRINT("\n");
    // DEBUG(delay(100));
    // DEBUG

    eepromAdress = packetAdress;
    nextFreeEepromAdress = static_cast<uint16_t>(packetAdress + packetSize);
}

bool Controller::RetrieveBareKeyboardKeysFromMemory(BareKeyboardKey **payloadAsBareKeys, uint16_t *amountOfKeys, uint16_t *packetAdress, uint16_t *packetSize)
{
    *amountOfKeys = *packetAdress = *packetSize = 0;
    DataPacket *dataPtr = new DataPacket();
    DataPacket packet = *dataPtr;

    bool foundValidPacket = false;
    while (!foundValidPacket)
    {
        uint16_t startAdress = *packetAdress;
        bool foundPacket = RetrieveDataPacketFromMemory(&packet, packetSize, packetAdress, startAdress);
        if (!foundPacket)
            return false;

        // DEBUG_PRINT("Began Converting DataPacket to Keymaps...\n"); // DEBUG
        // DEBUG(delay(100));                                                  // DEBUG

        *amountOfKeys = static_cast<uint16_t>(packet.payloadLength / sizeof(BareKeyboardKey));
        delete[](*payloadAsBareKeys);
        *payloadAsBareKeys = new BareKeyboardKey[*amountOfKeys];

        ConvertDataPacketToBareKeyboardKeys(packet, *payloadAsBareKeys);

        foundValidPacket = true;
        for (uint16_t i = 0; i < *amountOfKeys; i++)
        {
            bool isValid = IsKeyValid((*payloadAsBareKeys)[i].pin);

            DEBUG(
                DEBUG_PRINT("\nIsValid?: "); 
                DEBUG_PRINT(isValid); 
                DEBUG_PRINT("  {");
                DEBUG_PRINT(" .pin: ");
                DEBUG_PRINT((*payloadAsBareKeys)[i].pin);
                DEBUG_PRINT(", .keyCode: ");
                DEBUG_PRINT((*payloadAsBareKeys)[i].keyCode);
                DEBUG_PRINT(" }");
                DEBUG(delay(100));
            );
            if (!isValid)
            {
                foundValidPacket = false;
                *packetAdress = static_cast<uint16_t>(*packetAdress + *packetSize);
                break;
            }
        }
        DEBUG_PRINT("\n");
    }

    delete (dataPtr);
    return true;
}

bool Controller::RetrieveDataPacketFromMemory(DataPacket *packet, uint16_t *packetSize, uint16_t *packetAdress, uint16_t startAdress)
{
    *packetAdress = startAdress;
    *packetSize = 0;
    bool foundPacket = false;
    do
    {
        foundPacket = ParsePacketFromEEPROM(*packetAdress, packet, packetSize);
        if (!foundPacket)
        {
            *packetAdress = static_cast<uint16_t>(*packetAdress + 1);
            if (*packetAdress >= EEPROM.length())
            {
                DEBUG_PRINT("Failed to read data from memory!\n"); // DEBUG
                DEBUG(delay(100));                                         // DEBUG

                return false;
            }
        }
    } while (!foundPacket && *packetAdress < EEPROM.length());

    return foundPacket;
}

void Controller::ConvertDataPacketToBareKeyboardKeys(DataPacket packet, BareKeyboardKey *result)
{
    uint8_t* resultAsBytes = reinterpret_cast<uint8_t*>(result);
    for (unsigned int i = 0; i < packet.payloadLength; i++)
    {
        resultAsBytes[i] = packet.payload[i];
    }
}

void Controller::ParseBareKeyboardKeyArrayIntoKeymapList(BareKeyboardKey *keys, unsigned int amountOfKeys, LinkedList<BareKeyboardKey *> *keymapList) // NOTE: Refactored to BareKeyboardKeys.
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
            // DEBUG_PRINT("BareKey:\n");
            // DEBUG_PRINT("    .pin: ");
            // DEBUG_PRINT(currentKey.pin);
            // DEBUG_PRINT("\n    .keyCode: ");
            // DEBUG_PRINT(currentKey.keyCode);

            // DEBUG_PRINT("\nConvertedKey:\n");
            // DEBUG_PRINT("    .pin: ");
            // DEBUG_PRINT(keyMap[j].pin);
            // DEBUG_PRINT("\n    .keyCode: ");
            // DEBUG_PRINT(keyMap[j].keyCode);
            // DEBUG_PRINT("\n");
            // DEBUG(delay(100));
            // // DEBUG
        }
        keymapList->Add(keyMap);
    }
}

bool Controller::IsKeyValid(const IKey &pin)
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        if (pin == defaultKeymap[i].pin)
            return true;
    }

    return false;
}

void Controller::CycleKeyMap()
{
    if (customKeyMaps.IsEmpty())
    {
        // We can't cycle through an empty list of keymaps...
        DEBUG_PRINT("Empty\n"); // DEBUG
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

void Controller::ChangeKeyMap(BareKeyboardKey *keyMap)
{
    DEBUG_PRINT("Changing keymap\n"); // DEBUG
    // Overwrite currentKeyMap with the keys we want to equip.
    for(int i = 0; i < normalKeyCount; i++) 
    {
        currentKeyMap[i] = Key(keyMap[i].pin, keyMap[i].keyCode);
    }

    // Perform a key release for all active buttons.
    WipeKeyboardEventBuffer();
    SendKeyboardEvent();

    // Keep track if we are using the default keymap.
    isUsingDefaultKeymap = keyMap == defaultKeymap;

    // Configure pins
    ConfigurePinsForKeyMap(currentKeyMap, normalKeyCount);
}

void Controller::UpdateCurrentCustomKeymap()
{
    BareKeyboardKey *currentCustomKeymap = *(customKeyMaps[customKeyMapIndex]);

    for(int i = 0; i < normalKeyCount; i++) 
    {
        currentCustomKeymap[i] = BareKeyboardKey(currentKeyMap[i].pin, currentKeyMap[i].keyCode);
    }
}

void Controller::ToggleDefaultKeyMap() // NOTE: Refactored to BareKeyboardKeys // TODO: Needs to be tested.
{
    bool toggleToDefault = !isUsingDefaultKeymap;
    if (toggleToDefault)
    {
        ChangeKeyMap(defaultKeymap);
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
    for (int keyIndex = 0; keyIndex < normalKeyCount; keyIndex++)
    {
        Key &key = currentKeyMap[keyIndex];

        if (OnKeyPress(key.state))
        {
            digitalWrite(LED_BUILTIN, HIGH);

            // Find empty slot
            int index = 2; // 2 = Start position for keys.
            bool foundEmpty = false;
            for (int i = 2; i < bufferSize && !foundEmpty; i++) // TODO: Running through the forloop every keypress might not be efficient. Push pop list of empty positions? Construct a Dictionary?
            {
                foundEmpty = buf[i] == 0;
                if (foundEmpty)
                    index = i;
            }

            if (foundEmpty)
            {
                // Send keypress
                buf[index] = static_cast<uint8_t>(key.keyCode);
                SendKeyboardEvent();
            }
        }
        else if (OnKeyRelease(key.state))
        {
            digitalWrite(LED_BUILTIN, LOW);

            // Find empty slot
            int index = 2; // 2 = Start position for keys.
            bool foundKeyCode = false;
            for (int i = 2; i < bufferSize && !foundKeyCode; i++) // TODO: Running through the forloop every keypress might not be efficient. Push pop list of empty positions? Construct a Dictionary?
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
                for (int i = 2; i < bufferSize; i++) // TODO: Running through the forloop every keypress might not be efficient. Push pop list of empty positions? Construct a Dictionary?
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
    for(int i = 0; i < bufferSize; i++) 
    {
        buf[i] = 0;
    }
}

void Controller::SendKeyboardEvent() 
{
    Serial.write(buf, bufferSize);
}

void Controller::ExecuteSpecialCommands() // TODO: Needs to be tested. Refactor to use OnKeyPress and etc.
{
    for (int i = 0; i < specialKeyCount; i++)
    {
        SpecialKey &specialKey = specialKeys[i];

        if (specialKey.state.oldValue != specialKey.state.value)
        {
            if (specialKey.state.value)
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
                        if (OnLongPress(specialKey.state, longPressDuration))
                        {
                            // DEBUG_PRINT("Long press, released. Save to memory...\n"); // DEBUG
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
                        if (OnLongPress(specialKey.state, longPressDuration))
                        {
                            // DEBUG_PRINT("Long press, released. Delete keymap...\n"); // DEBUG
                            DeleteCurrentKeyMap();
                        }
                    }
                    break;
                }
                }

                //DEBUG_PRINT((millis() - specialKey.timeOfActivation)); // DEBUG
                //DEBUG_PRINT("\n");
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

    UpdateCurrentCustomKeymap();
}

void Controller::SaveControllerSettings() // TODO: Needs to be tested.
{
    UpdateCurrentCustomKeymap();
    SaveKeyMapsToMemory(customKeyMaps);

    uint32_t timeNeeded = customKeyMaps.length * normalKeyCount * sizeof(BareKeyboardKey) * 5;

    // // DEBUG
    // DEBUG_PRINT("Time needed to save: ");
    // DEBUG_PRINT(timeNeeded);
    // DEBUG_PRINT("\n");
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

    DEBUG(
        for (unsigned int i = 0; i < customKeyMaps.length; i++)
        {
            DEBUG_PRINT("Before deleting ");
            DEBUG_PRINT(i);
            DEBUG_PRINT(":\n");
            for (int j = 0; j < normalKeyCount; j++)
            {
                DEBUG_PRINT("    ( pin: ");
                DEBUG_PRINT((*customKeyMaps[i])[j].pin);
                DEBUG_PRINT(", keyCode: ");
                DEBUG_PRINT((*customKeyMaps[i])[j].keyCode);
                DEBUG_PRINT(" )\n");
            }
        }
        DEBUG(delay(100));
    );

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
            DEBUG_PRINT("Switched to default keymap\n"); // DEBUG
            ChangeKeyMap(defaultKeymap);
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
                DEBUG_PRINT(customKeyMapIndex);   // DEBUG
                DEBUG_PRINT("\n");
                ChangeKeyMap(*nextKeyMapPtr);
            }
            else
            {
                DEBUG_PRINT("Failed to delete keymap at "); // DEBUG
                DEBUG_PRINT(customKeyMapIndex);           // DEBUG
                DEBUG_PRINT("\n");
                // TODO: Throw error. We failed to retrieve the keymap at position customKeyMapIndex.
            }
        }

        if (*removedKeyMapPtr != nullptr) // TODO: Is this needed?
        {
            DEBUG_PRINT("Deleted pointer... *removedKeyMapPtr\n"); // DEBUG
            delete (*removedKeyMapPtr);
        }
    }
    else
    {
        // TODO: Throw error. We failed to delete the keyMap.
        DEBUG_PRINT("Something went really wrong..."); // DEBUG
        DEBUG_PRINT(customKeyMapIndex);              // DEBUG
        DEBUG_PRINT("\n");
    }

    delete (removedKeyMapPtr);                             // TODO: Check if this is correct or not.
    DEBUG_PRINT("Deleted pointer... removedKeyMapPtr\n"); // DEBUG
    ToggleEditMode();

    // DEBUG
    DEBUG_PRINT("Amount of keymaps left: ");
    DEBUG_PRINT(customKeyMaps.length);
    DEBUG_PRINT("\nCurrent position: ");
    DEBUG_PRINT(customKeyMapIndex);
    DEBUG_PRINT("\n");
    // DEBUG

    DEBUG(
        for (unsigned int i = 0; i < customKeyMaps.length; i++)
        {
            DEBUG_PRINT("After deleting ");
            DEBUG_PRINT(i);
            DEBUG_PRINT(":\n");
            for (int j = 0; j < normalKeyCount; j++)
            {
                DEBUG_PRINT("    ( pin: ");
                DEBUG_PRINT((*customKeyMaps[i])[j].pin);
                DEBUG_PRINT(", keyCode: ");
                DEBUG_PRINT((*customKeyMaps[i])[j].keyCode);
                DEBUG_PRINT(" )\n");
            }
        }
        if (customKeyMaps.length == 0)
        {
            DEBUG_PRINT("avaiableKeyMaps is Empty\n");
        }
        DEBUG(delay(100));
    );
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
            newKeyMap[i] = BareKeyboardKey(defaultKeymap[i].pin, initialKeycode);
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

            //DEBUG_PRINT("Created new keymap!\n"); // DEBUG
            //DEBUG(delay(100)); // DEBUG

            // DEBUG(
            //     DEBUG_PRINT("\n");
            //     DEBUG_PRINT("New current keymap:\n");
            //     for (int i = 0; i < normalKeyCount; i++)
            //     {
            //         DEBUG_PRINT("Current .pin = ");
            //         DEBUG_PRINT(currentKeyMap[i].pin);
            //         DEBUG_PRINT(", .keyCode = ");
            //         DEBUG_PRINT(currentKeyMap[i].keyCode);
            //         DEBUG_PRINT("\n");
            //     }
            //     DEBUG_PRINT("Amount of keymaps: ");
            //     DEBUG_PRINT(customKeyMaps.length);
            //     DEBUG_PRINT("\n");
            //     DEBUG(delay(100));
            // );
        }
        else
        {
            // DEBUG_PRINT("Something messed up\n"); // DEBUG
            // TODO: Error we failed to retrieve the newly added keymap.
        }
    }
    else
    {
        DEBUG_PRINT("We don't have enought space to create another keymap (Max size hardcoded to 10)...\n"); // DEBUG
        SignalErrorToUser(); // TODO: prevent getting thrown out of memory when we fail to add a new keymap.
        // TODO: Error we don't have space to create another keyMap.
    }

    return successful;
}

void Controller::SignalErrorToUser() // TODO: Needs to be tested?
{
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
    }
    return;
}