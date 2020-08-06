#include <Arduino.h>
//#include <EEPROM.h>

#include <Controller.h>

#define NORMAL_KEY_COUNT 4
#define SPECIAL_KEY_COUNT 3

// Public variables
const int normalKeyCount = NORMAL_KEY_COUNT;
Key defaultKeyMap[normalKeyCount] = {
    // Key map Arrow keys
    Key(2, 80),
    Key(3, 82),
    Key(4, 81),
    Key(5, 79),
};

const int specialKeyCount = SPECIAL_KEY_COUNT;
SpecialKey specialKeys[specialKeyCount] = {
    SpecialKey(10, toggleEditMode),
    SpecialKey(11, cycleKeyMap),
    SpecialKey(12, toggleDefaultKeyMap), // This one should never change.
};

extern Key *currentKeyMap;
extern LinkedList<Key *> customKeyMaps;
extern EditMode editmode;


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

void loop()
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

