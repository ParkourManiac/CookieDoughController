#include <Arduino.h>
#include <EEPROM.h>

#include <DataPacket.h>
#include <Key.h>
#include <LinkedList.h>
#include <LinkedList.cpp>

// HEADER
void SaveKeyMapsToMemory(LinkedList<Key *> &keyMapList);
void LoadKeyMapsFromMemory(LinkedList<Key *> &keyMapList);
void ConfigurePinsAsKeys();
void CycleKeyMap();
void ChangeKeyMap(Key *keyMap);
void ReadPinValueForKeys();
void SendKeyInfo();
void ExecuteSpecialCommands();
unsigned long CalculateCRC(uint8_t *data, unsigned int length);

// Public variables
const int normalKeyCount = 4;

Key defaultKeyMap[normalKeyCount] = {
    // Key map Arrow keys
    {.pin = 2, .keyCode = 80},
    {.pin = 3, .keyCode = 82},
    {.pin = 4, .keyCode = 81},
    {.pin = 5, .keyCode = 79},
};

SpecialKey specialKeys[2] = {
    {.pin = 11, .function = cycleKeyMap},
    {.pin = 12, .function = toggleDefaultKeyMap}, // This one should never change.
};

Key *currentKeyMap = defaultKeyMap;
int customKeyMapIndex = 0;
LinkedList<Key *>* availableKeyMapsPtr = new LinkedList<Key*>();
LinkedList<Key *> availableKeyMaps = *availableKeyMapsPtr;
//LinkedList<Key *> availableKeyMaps = *(new LinkedList<Key*>());

uint8_t buf[8] = {0}; // Keyboard report buffer.

unsigned int eepromAdress = 0;

// PROGRAM
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    // Key keys[normalKeyCount] = {};
    // availableKeyMaps.Add(keys);
    SaveKeyMapsToMemory(availableKeyMaps);

    availableKeyMaps.Clear();
    LoadKeyMapsFromMemory(availableKeyMaps);
    ConfigurePinsAsKeys();
}

void loop()
{
    ReadPinValueForKeys();
    ExecuteSpecialCommands();
    SendKeyInfo();
}

void SaveKeyMapsToMemory(LinkedList<Key *> &keyMapList) // TODO: Save something to EEPROM using data packet.
{
    unsigned int serializedSize = sizeof(Key[keyMapList.length * normalKeyCount]);
    Key *serializedKeyMaps = new Key[keyMapList.length * normalKeyCount];
    for (unsigned int i = 0; i < keyMapList.length; i++)
    {
        for (unsigned int j = 0; j < normalKeyCount; j++)
        {
            unsigned int pos = i * normalKeyCount + j;
            serializedKeyMaps[pos] = (*keyMapList[i])[j];
        }
    }

    uint8_t *dataPtr = (uint8_t *)serializedKeyMaps;
    unsigned int packetSize;
    bool success = SavePacketToEEPROM(eepromAdress, dataPtr, serializedSize, packetSize);
    if (!success)
    {
        Serial.println("Failed to write data to memory!");
        delay(100);
    }
    eepromAdress += packetSize;

    delay(500);
    delete (serializedKeyMaps);
}

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

        if (packetAdress == EEPROM.length())
        {
            Serial.println("Failed to read data from memory!");
            delay(100);
            return;
        }
    } while (!foundPacket && packetAdress < EEPROM.length());

    Serial.println("Began loading...");
    delay(100);

    // Serial.println(packet.stx, HEX);
    // Serial.println(packet.payloadLength);
    // Serial.println(packet.crc);

    // Convert
    unsigned int amountOfKeys = packet.payloadLength / sizeof(Key);
    Key payloadAsKeys[normalKeyCount * amountOfKeys];
    for (unsigned int i = 0; i < amountOfKeys; i++)
    {
        payloadAsKeys[i] = ((Key *)packet.payload)[i];
    }

    unsigned int amountOfKeymaps = amountOfKeys / normalKeyCount;
    for (unsigned int i = 0; i < amountOfKeymaps; i++) // For each keymap
    {
        Key *keyMap = new Key[normalKeyCount];
        for (unsigned int j = 0; j < normalKeyCount; j++) // For each key in a keymap
        {
            keyMap[j] = payloadAsKeys[i * normalKeyCount + j];
        }
        keyMapList.Add(keyMap);
    }

    // print
    Serial.println("Data:");
    for (unsigned int i = 0; i < keyMapList.length; i++)
    {
        Serial.println("{");
        for (unsigned int j = 0; j < normalKeyCount; j++)
        {
            Serial.print("    .pin: ");
            Serial.println((*keyMapList[i])[j].pin);
            Serial.print("    .keyCode: ");
            Serial.println((*keyMapList[i])[j].keyCode);
        }
        Serial.println("}");
    }
    delay(100);

    // Serial.println("DATA:::::");
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     Serial.println(packet.payload[i], HEX);
    // }
    // Serial.println(":::::");

    // Serial.println();
    // Serial.println(packet.etx, HEX);

    Serial.println("Finished loading.");

    Serial.println();
    Serial.print("Packet size: ");
    Serial.println(packetSize);
    delay(100);

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
    Key *nextKeyMap = *(availableKeyMaps[customKeyMapIndex]);

    ChangeKeyMap(nextKeyMap);
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
        ChangeKeyMap(defaultKeyMap);
    else
        ChangeKeyMap(*(availableKeyMaps[customKeyMapIndex]));
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
        key.value = !digitalRead(key.pin); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    }

    for (SpecialKey &specialKey : specialKeys)
    {
        specialKey.value = !digitalRead(specialKey.pin); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    }
}

/**
 * @brief Writes the keypress events to the buffer and sends them to the computer. 
 */
void SendKeyInfo()
{ // TODO: Handle debounce.
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
{ // TODO: Handle debounce.
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

// Miscellaneous

// Key setup WASD
// Key keys[4] = {
//     {.pin = 2, .keyCode = 4},
//     {.pin = 3, .keyCode = 26},
//     {.pin = 4, .keyCode = 22},
//     {.pin = 5, .keyCode = 7},
// };

// Key setup Arrow keys
// Key keys[4] = {
//     {.pin = 2, .keyCode = 80},
//     {.pin = 3, .keyCode = 82},
//     {.pin = 4, .keyCode = 81},
//     {.pin = 5, .keyCode = 79},
// };