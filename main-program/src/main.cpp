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
void ToggleEditMode();
void ResetEditMode();
void EditMode();
void debounceRead(IPinState &key);

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
int customKeyMapIndex = 0;
LinkedList<Key *> *availableKeyMapsPtr = new LinkedList<Key *>();
LinkedList<Key *> availableKeyMaps = *availableKeyMapsPtr;

uint8_t buf[8] = {0}; // Keyboard report buffer.

unsigned int eepromAdress = 0;

bool editmode = false;
Key *editmodeSelectedKey;
int editmodeKeysPressed = 0;
int editmodeKeyCode = 0;
bool editmodeShouldAddValue;

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
    // eepromAdress = 50;
    // availableKeyMaps.Add(keys);
    // SaveKeyMapsToMemory(availableKeyMaps);

    availableKeyMaps.Clear();
    LoadKeyMapsFromMemory(availableKeyMaps);
    ConfigurePinsAsKeys();
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
            // TODO: maybe we need to reset the key value and old value before saving?
            // Maybe we should break apart {key.value, key.oldValue} from {key.pin, key.keycode}?

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
    bool success = SavePacketToEEPROM(eepromAdress, dataPtr, serializedSize, packetSize);
    if (!success)
    {
        // Serial.println("Failed to write data to memory!");
        // delay(100);

        // TODO: Implement error code.
    }
    eepromAdress += packetSize;

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
    BareKeyboardKey payloadAsKeys[normalKeyCount * amountOfKeys];
    for (unsigned int i = 0; i < amountOfKeys; i++)
    {
        payloadAsKeys[i] = ((BareKeyboardKey *)packet.payload)[i];
    }

    unsigned int amountOfKeymaps = amountOfKeys / normalKeyCount;
    for (unsigned int i = 0; i < amountOfKeymaps; i++) // For each keymap
    {
        Key *keyMap = new Key[normalKeyCount];
        for (unsigned int j = 0; j < normalKeyCount; j++) // For each key in a keymap
        {
            BareKeyboardKey currentKey = payloadAsKeys[i * normalKeyCount + j];
            keyMap[j].pin = currentKey.pin;
            keyMap[j].keyCode = currentKey.keyCode;
        }
        keyMapList.Add(keyMap);
    }

    // // print
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

    eepromAdress = packetAdress + packetSize;
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
    // TODO: If default is selected, don't increment.
    // Switch to the same normal keymap.

    // If we are using the default. Switch back to the
    // previous keymap. Otherwise move to the next.
    bool isDefault = (currentKeyMap == defaultKeyMap);
    int nextIndex = (isDefault) ? customKeyMapIndex : customKeyMapIndex + 1;

    customKeyMapIndex = nextIndex % availableKeyMaps.length;
    Key **nextKeyMapPtr = availableKeyMaps[customKeyMapIndex];

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
    if (currentKeyMap != defaultKeyMap)
    {
        ChangeKeyMap(defaultKeyMap);
    }
    else
    {
        Key **keyMapPtr = availableKeyMaps[customKeyMapIndex];
        if (keyMapPtr != nullptr)
        {
            ChangeKeyMap(*(keyMapPtr));
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
        debounceRead(key);
    }

    for (SpecialKey &specialKey : specialKeys) // TODO: Handle debounce.
    {
        debounceRead(specialKey); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
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
        if (key.oldValue != key.value)
        {
            if (key.value)
            {
                digitalWrite(LED_BUILTIN, HIGH);

                // Send keypress
                buf[2] = key.keyCode;
                Serial.write(buf, 8);
            }
            else
            {
                digitalWrite(LED_BUILTIN, LOW);

                // Send release keypress
                buf[0] = 0;
                buf[2] = 0;
                Serial.write(buf, 8);
            }
        }

        key.oldValue = key.value;
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
                    ToggleEditMode();
                    break;
                case cycleKeyMap:
                    CycleKeyMap();
                    break;
                case toggleDefaultKeyMap:
                    ToggleDefaultKeyMap();
                    break;
                }
            }
            else
            {
                digitalWrite(LED_BUILTIN, LOW);
            }
        }

        specialKey.oldValue = specialKey.value;
    }
}

void ToggleEditMode()
{
    // Prevent editing the default key map.
    if (currentKeyMap == defaultKeyMap)
        return;


    editmode = !editmode;
    if (editmode)
    { 
        // Reset editmode before it starts.
        ResetEditMode();
    }
}

void ResetEditMode() {
    editmodeSelectedKey = nullptr;
    editmodeKeysPressed = 0;
    editmodeKeyCode = 0;
}

void EditMode()
{
    for (int i = 0; i < normalKeyCount; i++)
    {
        Key &key = currentKeyMap[i];
        if (key.oldValue != key.value)
        {
            if (key.value)
            {
                digitalWrite(LED_BUILTIN, HIGH);

                // Keypress
                editmodeKeysPressed += 1;

                if(editmodeSelectedKey == nullptr) 
                {
                    editmodeSelectedKey = &key;
                }

                if(!editmodeShouldAddValue)
                {
                    editmodeShouldAddValue = true;
                }
            }
            else
            {
                digitalWrite(LED_BUILTIN, LOW);

                // Keyrelease
                if(editmodeSelectedKey != nullptr) 
                {
                    if(editmodeShouldAddValue) 
                    {
                        editmodeShouldAddValue = false;
                        // If more than one key was held down when letting this one go...
                        if(editmodeKeysPressed >= 2) 
                        {
                            // Add raise value of keycode.
                            int exponent = editmodeKeysPressed - 2;
                            int numberToAdd = pow(10, exponent);
                            
                            // editmodeKeyCode += numberToAdd;
                            // Serial.print("Inputed keycode: ");
                            // Serial.print(editmodeKeyCode);
                            // Serial.print(", for pin: ");
                            // Serial.print(editmodeSelectedKey->pin);
                            // Serial.print(", keycode: ");
                            // Serial.println(editmodeSelectedKey->keyCode);
                        }
                    }

                    // If we are letting go of the last key...
                    if(editmodeKeysPressed == 1) 
                    {
                        editmodeSelectedKey->keyCode = editmodeKeyCode;


                        // Serial.println("Applied keycode to key.");
                        // Serial.print("Updated key: .pin = ");
                        // Serial.print(editmodeSelectedKey->pin);
                        // Serial.print(", .keyCode = ");
                        // Serial.println(editmodeSelectedKey->keyCode);
                    }
                }

                editmodeKeysPressed -= 1;
            }

            // Serial.println(editmodeKeysPressed);
            // TODO: Reset if we are not pressing any keys.
            if(editmodeKeysPressed == 0) {
                ResetEditMode();
            }
        }

        key.oldValue = key.value;
    }
}

/**
 * @brief Reads and updates the value of a
 * keys pin with the debounced input.
 * 
 * @param key The key to be updated.
 */
void debounceRead(IPinState &key) // TODO: This causes a slight input delay. Consider this: if you were to press the button every <30ms the input would not be registered.
{
    unsigned int debounceDelay = 30; // TODO: This balance needs to be play tested.

    // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    bool pinState = !digitalRead(key.pin);

    // If the pin state has changed...
    if (pinState != key.oldPinState)
    {
        key.lastDebounceTime = millis();
        // // Print debounce catches.
        // Serial.print("he");
    }

    unsigned long timePassedSinceDebounce = (millis() - key.lastDebounceTime);
    // If we've waited long enough since last debounce...
    if (timePassedSinceDebounce > debounceDelay)
    {
        // And if the old state is not already the new state...
        if (pinState != key.value)
        {
            key.value = pinState;

            // // Print debounce catches.
            // if(key.value) {
            //     Serial.print(" hej");
            // } else {Serial.print(" hå");}
            // Serial.println();
        }
    }

    key.oldPinState = pinState;
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