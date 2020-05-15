#include <Arduino.h>
#include <EEPROM.h>

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

struct DataPacket
{
    uint8_t stx = 0x02;
    uint16_t payloadLength;
    uint32_t crc;
    uint8_t *payload;
    uint8_t etx = 0x03;
};

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
/** TODO: DOUBLE CHECK THIS DESCRIPTION WHEN FINISHED IMPLEMENTING FUNCTIONALITY.
 * @brief Reads and parses a data packet from the eeprom memory.
 * 
 * @param eepromAdress The starting adress of the packet.
 * @param parsedPacket The output data packet.
 * @return true If we successfully read the data packet from memory.
 * @return false If we were unsuccessful in reading the data
 * packet from memory.
 */
bool ParsePacketFromEEPROM(unsigned int adress, DataPacket &parsedPacket) // TODO: Needs to be tested.
{
    // Verifera att eepromAdress == parsedPacket.stx
    // Läs in paketets längd
    // Läs in paketets checksumma crc
    // Verifera att längden längden är rimlig ;)
    // Läs in payloaden.
    // Dubbelkolla att nästa byte är packet.etx.
    // Beräkna crc och jämför med parcedPacket.crc.
    // Skriv till parsedPacket.
    // returnera true.
    // Om något ovanstående inte stämmer, returnera false.
}

/**
 * @brief Saves a data packet to memory.
 * 
 * @param adress The destination adress for the packet to be written.
 * @param data The data/payload to saved.
 * @param sizeOfPacket The total size of the data packet.
 * @return true If we successfully wrote the data packet to memory.
 * @return false If we were unsuccessful in writing the data packet to memory
 */
bool SavePacketToEEPROM(unsigned int adress, uint8_t *data, unsigned int &sizeOfPacket)// TODO: NEEDS TO BE TESTED
{ 
    sizeOfPacket = 0;
    unsigned int initAdress = adress;

    // Create packet.
    DataPacket packet;
    packet.payload = data;
    packet.payloadLength = sizeof(packet.payload);
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);

    // Write packet.
    EEPROM.put(adress, packet.stx);
    adress += sizeof(packet.stx);
    EEPROM.put(adress, packet.payloadLength);
    adress += sizeof(packet.payloadLength);
    EEPROM.put(adress, packet.crc);
    adress += sizeof(packet.crc);
    for (unsigned int i = 0; i < packet.payloadLength; i++)
    {
        EEPROM.update(adress, packet.payload[i]);
        adress += sizeof(packet.payload[i]);
    }
    EEPROM.put(adress, packet.etx);
    adress += sizeof(packet.etx);

    // Verify that package can be read from memory correctly.
    DataPacket packetFromEeprom;
    bool success = ParsePacketFromEEPROM(initAdress, packetFromEeprom);
    if (!success || packet.crc != packetFromEeprom.crc)
    {
        return false; // Something went wrong when writing.
    }

    sizeOfPacket = adress - initAdress;
    return true; // Package saved successfully.
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

    delay(100);
}

void LoadKeyMapsFromMemory() // TODO: Load availableKeyMaps from EEPROM.
{
    // EEPROM.length();
    // EEPROM.get();

    unsigned int adress = 0;
    do
    {
        int value;
        EEPROM.get(adress, value);
        Serial.println(value);
        adress += sizeof(value);
    } while (adress < EEPROM.length());

    char a[] = "Hello world";
    Serial.println("");
    Serial.println(CalculateCRC((uint8_t *)a, sizeof(a)));
    Serial.println(73897685 == CalculateCRC((uint8_t *)a, sizeof(a)));
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

/**
 * @brief Calculates a CRC checksum from the provided data.
 * NOTE: This is a modified version of the CRC function 
 * at arduino page: https://www.arduino.cc/en/Tutorial/EEPROMCrc
 * 
 * @param data The data that will be used to calculate the CRC checksum.
 * @param length The length of the data in bytes.
 * @return unsigned long Returns a CRC checksum.
 */
unsigned long CalculateCRC(uint8_t *data, unsigned int length)
{

    const unsigned long crc_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c};

    unsigned long crc = ~0L;

    for (unsigned int index = 0; index < length; ++index)
    {
        crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
        crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
        crc = ~crc;
    }
    return crc;
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