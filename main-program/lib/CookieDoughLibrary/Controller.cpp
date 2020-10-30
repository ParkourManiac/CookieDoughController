#include "Controller.h"
#include "MeasureSRAM.h"

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
    , storageSize(EEPROM.length())
    , amountOfFreeStorage(this->storageSize)
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
    // // DEBUG "CLEAN EEPROM"
    // for(unsigned int i = 0; i < EEPROM.length(); i++) {
    //     EEPROM.write(i, 0);
    // }
    // // DEBUG
    
    // // DEBUG "DeactivateAllPacketsOnEEPROM"
    // DeactivateAllPacketsOnEEPROM();
    // // DEBUG

    // // DEBUG "ReadBytesFromEEPROM"
    // const uint16_t amountOfBytes = 10;
    // uint8_t result[amountOfBytes] = {0};
    // bool resultBool = ReadBytesFromEEPROM(10, amountOfBytes, result);
    // if(resultBool)
    // {
    //     for(uint16_t i = 0; i < amountOfBytes; i++)
    //     {
    //         DEBUG_PRINT(result[i]);
    //         DEBUG_PRINT(F("|"));
    //     }
    // DEBUG_PRINT(F("\n\n"));
    // }
    // // DEBUG

    // DEBUG(
    //     DEBUG_PRINT(F("|"));
    //     for(unsigned int i = 0; i < EEPROM.length(); i++)
    //     {
    //         uint8_t current = EEPROM.read(i);
    //         DEBUG_PRINT(current);
    //         DEBUG_PRINT(F("|"));
    //     }
    // )

    // // DEBUG "JoinDataWithDataPacketOnEEPROM"
    // BareKeyboardKey data[normalKeyCount] {
    //     BareKeyboardKey(defaultKeymap[0].pin, 99),
    //     BareKeyboardKey(defaultKeymap[1].pin, 99),
    //     BareKeyboardKey(defaultKeymap[2].pin, 99),
    //     BareKeyboardKey(defaultKeymap[3].pin, 99),
    // };
    // uint8_t *dataPtr = reinterpret_cast<uint8_t *>(&data);
    // DEBUG_PRINT(JoinDataWithDataPacketOnEEPROM(storageSize, dataPtr, sizeof(data)));
    // DEBUG_PRINT(F("\n"));
    // delay(5000);
    // // DEBUG
    

    DEBUG_PRINT(F("\nChanging to default keymap.\n"));
    DEBUG(delay(100));
    ChangeKeyMap(defaultKeymap);
    LoadKeymapsFromMemoryIntoList(&customKeyMaps);
    ConfigurePinsForKeyMap<Key>(currentKeyMap, normalKeyCount);
    ConfigurePinsForKeyMap<SpecialKey>(specialKeys, specialKeyCount);
}

void Controller::Update()
{
    UpdatePinStatesForKeyMap(currentKeyMap, normalKeyCount);
    UpdatePinStatesForKeyMap(specialKeys, specialKeyCount);

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

uint16_t Controller::CyclicEepromAdress(uint32_t adress)
{
    return CyclicAdress(adress, storageSize);
}

int32_t Controller::CalculateUnusedStorage(uint16_t amountOfKeymaps)
{
    if(amountOfKeymaps == 0)
    {
        amountOfKeymaps = static_cast<uint16_t>(customKeyMaps.length);
    }

    uint16_t sizeOfPayload = static_cast<uint16_t>(
        amountOfKeymaps * normalKeyCount * sizeof(BareKeyboardKey)
    );
    uint16_t sizeOfEmptyDataPacket = SizeOfSerializedDataPacket(DataPacket());

    return static_cast<int32_t>(
        storageSize - (sizeOfPayload + sizeOfEmptyDataPacket)
    );
}

bool Controller::SaveKeyMapsToMemory(const LinkedList<BareKeyboardKey *> &keymapList)
{
    const int keyCount = keymapList.length * normalKeyCount;
    DEBUG_PRINT(CalculateUnusedStorage(keymapList.length));
    DEBUG(delay(100));
    BareKeyboardKey *serializedKeyMaps = new BareKeyboardKey[keyCount];
    for (unsigned int i = 0; i < keymapList.length; i++)
    {
        for (int j = 0; j < normalKeyCount; j++)
        {
            unsigned int pos = i * normalKeyCount + j;
            serializedKeyMaps[pos] = (*keymapList[i])[j];
        }
    }

    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(serializedKeyMaps);
    uint16_t dataSize = static_cast<uint16_t>(sizeof(serializedKeyMaps[0]) * keyCount);
    // // DEBUG
    // DEBUG_PRINT(F("Passed in: "));
    // for(int i = 0; i < dataSize; i++) 
    // {
    //     DEBUG_PRINT(dataPtr[i], HEX);
    // }
    // DEBUG_PRINT(F("\n"));
    // DEBUG(delay(100));
    // // DEBUG
    uint16_t packetSize;
    bool success = SaveDataPacketToEEPROM(nextPacketAdress, dataPtr, dataSize, &packetSize);
    if (success)
    {
        DEBUG_PRINT(F("Settings saved!\n")); // DEBUG
        DEBUG(delay(100)); // DEBUG
        if(currentPacketAdress != nextPacketAdress) 
        {
            DeactivatePacket(currentPacketAdress);
        }
        currentPacketAdress = nextPacketAdress;
        nextPacketAdress = CyclicEepromAdress(currentPacketAdress + packetSize);
        amountOfFreeStorage = static_cast<uint16_t>(storageSize - packetSize);
    } else 
    {
        DEBUG_PRINT(F("ERROR: Failed to write data to memory!\n"));
        DEBUG(delay(100));

        // TODO: Implement error code.
    }

    delete[] (serializedKeyMaps);
    return success;
}

void Controller::LoadKeymapsFromMemoryIntoList(LinkedList<BareKeyboardKey *> *keymapList)
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
            DEBUG_PRINT(F("Data "));
            DEBUG_PRINT(i);
            DEBUG_PRINT(F(":\n"));
            for (int j = 0; j < normalKeyCount; j++)
            {
                DEBUG_PRINT(F("    ( pin: "));
                DEBUG_PRINT((*(*keymapList)[i])[j].pin);
                DEBUG_PRINT(F(", keyCode: "));
                DEBUG_PRINT((*(*keymapList)[i])[j].keyCode);
                DEBUG_PRINT(F(" )\n"));
            }
        }
        DEBUG(delay(100));
    );

    // DEBUG
    // DEBUG_PRINT(F("DATA:::::\n"));
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     DEBUG_PRINT(packet.payload[i], HEX);
    //     DEBUG_PRINT(F("\n"));
    // }
    // DEBUG_PRINT(F(":::::\n"));

    // DEBUG_PRINT(F("Finished loading.\n"));

    // DEBUG_PRINT(F("\n"));
    // DEBUG_PRINT(F("Packet size: "));
    // DEBUG_PRINT(packetSize);
    // DEBUG_PRINT(F("\n"));
    // DEBUG(delay(100));
    // DEBUG

    currentPacketAdress = packetAdress;
    nextPacketAdress = CyclicEepromAdress(packetAdress + packetSize);
    amountOfFreeStorage = static_cast<uint16_t>(storageSize - packetSize);
}

bool Controller::RetrieveBareKeyboardKeysFromMemory(BareKeyboardKey **payloadAsBareKeys, uint16_t *amountOfKeys, uint16_t *packetAdress, uint16_t *packetSize)
{
    *amountOfKeys = *packetAdress = *packetSize = 0;
    DataPacket packet;

    bool foundValidPacket = false;
    while (!foundValidPacket)
    {
        uint16_t startAdress = *packetAdress;
        bool foundPacket = false; //FindFirstDataPacketOnEEPROM(startAdress, &packet, packetSize, packetAdress); // TODO: Check that the order of the arguments is correct.
        if (!foundPacket)
            return false;

        // DEBUG_PRINT(F("Began Converting DataPacket to Keymaps...\n")); // DEBUG
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
                DEBUG_PRINT(F("\nIsValid?: ")); 
                DEBUG_PRINT(isValid); 
                DEBUG_PRINT(F("  {"));
                DEBUG_PRINT(F(" .pin: "));
                DEBUG_PRINT((*payloadAsBareKeys)[i].pin);
                DEBUG_PRINT(F(", .keyCode: "));
                DEBUG_PRINT((*payloadAsBareKeys)[i].keyCode);
                DEBUG_PRINT(F(" }"));
                DEBUG(delay(100));
            );
            if (!isValid)
            {
                foundValidPacket = false;
                *packetAdress = static_cast<uint16_t>(*packetAdress + *packetSize);
                break;
            }
        }
        DEBUG_PRINT(F("\n"));
    }
    
    return true;
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
            // DEBUG_PRINT(F("BareKey:\n"));
            // DEBUG_PRINT(F("    .pin: "));
            // DEBUG_PRINT(currentKey.pin);
            // DEBUG_PRINT(F("\n    .keyCode: "));
            // DEBUG_PRINT(currentKey.keyCode);

            // DEBUG_PRINT(F("\nConvertedKey:\n"));
            // DEBUG_PRINT(F("    .pin: "));
            // DEBUG_PRINT(keyMap[j].pin);
            // DEBUG_PRINT(F("\n    .keyCode: "));
            // DEBUG_PRINT(keyMap[j].keyCode);
            // DEBUG_PRINT(F("\n"));
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
        DEBUG_PRINT(F("Empty\n")); // DEBUG
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
    DEBUG_PRINT(F("Changing keymap\n")); // DEBUG
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
                        CreateNewKeymap();
                        ToggleEditMode(); // Enter it again so we can edit the newly created keymap.
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
                default:
                {
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
                            // DEBUG_PRINT(F("Long press, released. Save to memory...\n")); // DEBUG
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
                            // DEBUG_PRINT(F("Long press, released. Delete keymap...\n")); // DEBUG
                            DeleteCurrentKeyMap();
                        }
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }

                //DEBUG_PRINT((millis() - specialKey.timeOfActivation)); // DEBUG
                //DEBUG_PRINT(F("\n"));
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
            bool success = CreateNewKeymap();
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
    // DEBUG_PRINT(F("Time needed to save: "));
    // DEBUG_PRINT(timeNeeded);
    // DEBUG_PRINT(F("\n"));
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

void Controller::DeleteCurrentKeyMap() // NOTE: Refactored to BareKeyboardKeys. // TODO: Needs to be tested. // TODO: // TODO: After being tested, Double-check keymaps allocated in CreateNewKeymap is deleted[] when removing a keymap.
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
            DEBUG_PRINT(F("Before deleting "));
            DEBUG_PRINT(i);
            DEBUG_PRINT(F(":\n"));
            for (int j = 0; j < normalKeyCount; j++)
            {
                DEBUG_PRINT(F("    ( pin: "));
                DEBUG_PRINT((*customKeyMaps[i])[j].pin);
                DEBUG_PRINT(F(", keyCode: "));
                DEBUG_PRINT((*customKeyMaps[i])[j].keyCode);
                DEBUG_PRINT(F(" )\n"));
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
            DEBUG_PRINT(F("Switched to default keymap\n")); // DEBUG
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
                DEBUG_PRINT(F("Switched to keymap ")); // DEBUG
                DEBUG_PRINT(customKeyMapIndex);   // DEBUG
                DEBUG_PRINT(F("\n"));
                ChangeKeyMap(*nextKeyMapPtr);
            }
            else
            {
                DEBUG_PRINT(F("Failed to delete keymap at ")); // DEBUG
                DEBUG_PRINT(customKeyMapIndex);           // DEBUG
                DEBUG_PRINT(F("\n"));
                // TODO: Throw error. We failed to retrieve the keymap at position customKeyMapIndex.
            }
        }

        if (*removedKeyMapPtr != nullptr) // TODO: Is this needed?
        {
            DEBUG_PRINT(F("Deleted pointer... *removedKeyMapPtr\n")); // DEBUG
            delete (*removedKeyMapPtr);
        }
    }
    else
    {
        // TODO: Throw error. We failed to delete the keyMap.
        DEBUG_PRINT(F("Something went really wrong...")); // DEBUG
        DEBUG_PRINT(customKeyMapIndex);              // DEBUG
        DEBUG_PRINT(F("\n"));
    }

    delete (removedKeyMapPtr);                             // TODO: Check if this is correct or not.
    DEBUG_PRINT(F("Deleted pointer... removedKeyMapPtr\n")); // DEBUG
    ToggleEditMode();

    // DEBUG
    DEBUG_PRINT(F("Amount of keymaps left: "));
    DEBUG_PRINT(customKeyMaps.length);
    DEBUG_PRINT(F("\nCurrent position: "));
    DEBUG_PRINT(customKeyMapIndex);
    DEBUG_PRINT(F("\n"));
    // DEBUG

    DEBUG(
        for (unsigned int i = 0; i < customKeyMaps.length; i++)
        {
            DEBUG_PRINT(F("After deleting "));
            DEBUG_PRINT(i);
            DEBUG_PRINT(F(":\n"));
            for (int j = 0; j < normalKeyCount; j++)
            {
                DEBUG_PRINT(F("    ( pin: "));
                DEBUG_PRINT((*customKeyMaps[i])[j].pin);
                DEBUG_PRINT(F(", keyCode: "));
                DEBUG_PRINT((*customKeyMaps[i])[j].keyCode);
                DEBUG_PRINT(F(" )\n"));
            }
        }
        if (customKeyMaps.length == 0)
        {
            DEBUG_PRINT(F("avaiableKeyMaps is Empty\n"));
        }
        DEBUG(delay(100));
    );
}

bool Controller::CreateNewKeymap()
{
    bool successful = false;
    // TODO: Implement real check to see if the arduino can
    // fit another keymap to stack/heap/memory.
    int32_t freeStorageSpaceAfterAddingKeymap = CalculateUnusedStorage(static_cast<uint16_t>(customKeyMaps.length + 1));
    int32_t freeSRAMAfterAddingKeymap = freeMemory() - (sizeof(BareKeyboardKey) * normalKeyCount); // TODO: Replace this with another solution?
    bool canFitAnotherKeymapIntoStorage = (freeStorageSpaceAfterAddingKeymap >= 0);
    bool canFitAnotherKeymapIntoSRAM = (freeSRAMAfterAddingKeymap >= SRAMSafetyThreshold);

    if (canFitAnotherKeymapIntoStorage && canFitAnotherKeymapIntoSRAM)
    {
        BareKeyboardKey *newKeyMap = new BareKeyboardKey[normalKeyCount]; // TODO: Double-check if this is deleted[] when removing a keymap.
        int initialKeycode = 4; // The "a" key. 
        // Copy the default pin values to the new keyMap.
        for (int i = 0; i < normalKeyCount; i++)
        {
            newKeyMap[i] = BareKeyboardKey(defaultKeymap[i].pin, initialKeycode);
        }

        // Add it to the list and set it to the current keymap.
        customKeyMaps.Add(newKeyMap);
        int indexOfNewKeyMap = customKeyMaps.length - 1;
        BareKeyboardKey *lastKeyMap = (customKeyMaps[indexOfNewKeyMap] != nullptr) ? 
                                         *customKeyMaps[indexOfNewKeyMap] : nullptr;
        if (lastKeyMap != nullptr)
        {
            ChangeKeyMap(lastKeyMap);
            customKeyMapIndex = indexOfNewKeyMap;

            successful = true;

            //DEBUG_PRINT(F("Created new keymap!\n")); // DEBUG
            //DEBUG(delay(100)); // DEBUG

            // DEBUG(
            //     DEBUG_PRINT(F("\n"));
            //     DEBUG_PRINT(F("New current keymap:\n"));
            //     for (int i = 0; i < normalKeyCount; i++)
            //     {
            //         DEBUG_PRINT(F("Current .pin = "));
            //         DEBUG_PRINT(currentKeyMap[i].pin);
            //         DEBUG_PRINT(F(", .keyCode = "));
            //         DEBUG_PRINT(currentKeyMap[i].keyCode);
            //         DEBUG_PRINT(F("\n"));
            //     }
            //     DEBUG_PRINT(F("Amount of keymaps: "));
            //     DEBUG_PRINT(customKeyMaps.length);
            //     DEBUG_PRINT(F("\n"));
            //     DEBUG(delay(100));
            // );
        }
        else
        {
            DEBUG_PRINT(F("ERROR: Something messed up. We failed to retrieve the newly added keymap?\n"));
        }
    }
    else
    {
        DEBUG_PRINT(F("ERROR: We don't have enought space to create another keymap...\n"));
        SignalErrorToUser(); // TODO: prevent getting thrown out of memory when we fail to add a new keymap.
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
