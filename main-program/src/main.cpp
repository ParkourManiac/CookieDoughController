#include <Arduino.h>
#include <EEPROM.h>

#include <DataPacket.h>
#include <Key.h>
#include <LinkedList.h>
#include <LinkedList.cpp>

// HEADER
void SaveKeyMapsToMemory();
void LoadKeyMapsFromMemory();
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
LinkedList<Key *> availableKeyMaps;

uint8_t buf[8] = {0}; // Keyboard report buffer.

unsigned int eepromAdress = 0;

// PROGRAM
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    SaveKeyMapsToMemory(); // TODO: REMOVE THIS.
    LoadKeyMapsFromMemory();
    ConfigurePinsAsKeys();
}

void loop()
{
    ReadPinValueForKeys();
    ExecuteSpecialCommands();
    SendKeyInfo();
}

void SaveKeyMapsToMemory() // TODO: Save something to EEPROM using data packet.
{
    // delay(1000);
    // EEPROM.put(eepromAdress, (int)0);
    // eepromAdress += sizeof(int);
    // EEPROM.put(eepromAdress, (int)1337);
    // eepromAdress += sizeof(int);
    // EEPROM.put(eepromAdress, (int)32767);
    // eepromAdress += sizeof(int);

    // int data[8] = {1, 3, 3, 7, 4, 2, 0, 0};
    // uint8_t* dataPtr = (uint8_t*) &data;
    // unsigned int packetSize;
    // if(!SavePacketToEEPROM(eepromAdress, dataPtr, sizeof(data), packetSize))
    //     Serial.println("Failed to write data to memory!");
    // eepromAdress += packetSize;

    delay(1000);
}

void LoadKeyMapsFromMemory() // TODO: Load availableKeyMaps from EEPROM.
{
    DataPacket packet;
    unsigned int packetSize;

    if (ParsePacketFromEEPROM(0, packet, packetSize))
    {
        Serial.println(packet.stx, HEX);
        Serial.println(packet.payloadLength);
        Serial.println(packet.crc);
        Serial.println("Data:");
        for(int i = 0; i < 8; i ++) {
            Serial.print(((int *)packet.payload)[i]);
        }
        Serial.println();
        Serial.println(packet.etx, HEX);
    }
    else
    {
        Serial.println("Failed to read data from memory!");
    }

    Serial.println();
    Serial.println(packetSize);
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