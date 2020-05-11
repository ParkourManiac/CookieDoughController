#include <Arduino.h>
#include <Key.h>
#include <LinkedList.h>
#include <LinkedList.cpp>

// HEADER
void LoadKeyMaps();
void ConfigurePinsAsKeys();
void CycleKeyMap();
void ChangeKeyMap(int index);
void ReadPinValueForKeys();
void SendKeyInfo();
void ExecuteSpecialCommands();

// Public variables
int amountOfCustomizableKeys = 4;

Key defaultKeyMap[4] = { // Key map Arrow keys
    {.pin = 2, .keyCode = 4},
    {.pin = 3, .keyCode = 26},
    {.pin = 4, .keyCode = 22},
    {.pin = 5, .keyCode = 7},
};

Key* currentKeyMap = &defaultKeyMap[0]; // TODO: Continue converting things to pointers.
int customKeyMapIndex = 0;

LinkedList<Key[4]> availableKeyMaps;

SpecialKey specialKeys[1] = { // Should never change.
    {.pin = 12, .function = cycleKeyMap}
};

uint8_t buf[8] = {0}; // Keyboard report buffer.

// PROGRAM
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    LoadKeyMaps();
    ConfigurePinsAsKeys();
}

void loop()
{
    ReadPinValueForKeys();
    ExecuteSpecialCommands();
    SendKeyInfo();
}


void LoadKeyMaps() { // MOCKUP: TODO, change this into loading from memory.
    Key keyMapWASD[4] = {   // Key map WASD
            {.pin = 2, .keyCode = 80},
            {.pin = 3, .keyCode = 82},
            {.pin = 4, .keyCode = 81},
            {.pin = 5, .keyCode = 79},
    };
        
    Key keyMapArrows[4] = { // Key map Arrow keys
            {.pin = 2, .keyCode = 4},
            {.pin = 3, .keyCode = 26},
            {.pin = 4, .keyCode = 22},
            {.pin = 5, .keyCode = 7},
    };

    availableKeyMaps.Add(keyMapWASD);
    availableKeyMaps.Add(keyMapArrows);
}

/**
 * @brief Configures pins marked as Key or SpecialKey to act as input pins with internal pullups.
 */
void ConfigurePinsAsKeys() {
    for(int i = 0; i < amountOfCustomizableKeys; i++) {
        Key key = currentKeyMap[i];
        pinMode(key.pin, INPUT_PULLUP);
    }

    for(SpecialKey& specialKey : specialKeys) {
        pinMode(specialKey.pin, INPUT_PULLUP);
    }
}

/**
 * @brief Switches to the next keyMap configuration in the available keyMaps.
 * 
 */
void CycleKeyMap() {
    ChangeKeyMap(customKeyMapIndex + 1);
}

/**
 * @brief Changes the current keymap to the keymap specified at the given index (in the available keyMaps).
 * 
 * @param index The index of the keymap to be switched to.
 */
void ChangeKeyMap(int index) {
    customKeyMapIndex = index % availableKeyMaps.length;

    ConfigurePinsAsKeys();
}

/**
 * @brief Reads and updates the pin values of
 *  the current keymap and special keys.
 */
void ReadPinValueForKeys()
{
    for(int i = 0; i < amountOfCustomizableKeys; i++) {
        Key key = currentKeyMap[i];
        key.value = !digitalRead(key.pin); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    }

    for(SpecialKey& specialKey : specialKeys) {
        specialKey.value = !digitalRead(specialKey.pin); // Invert input signal. Pullup is active low. 1 = off. 0 = on.
    }
}

/**
 * @brief Writes the keypress events to the buffer and sends them to the computer. 
 * 
 */
void SendKeyInfo() { // TODO: Handle debounce.
    for(int i = 0; i < amountOfCustomizableKeys; i++) {
        Key key = currentKeyMap[i];
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
void ExecuteSpecialCommands() { // TODO: Handle debounce.
    for(SpecialKey& specialKey : specialKeys) {
        if (specialKey.oldValue != specialKey.value)
        {
            if (specialKey.value)
            {
                digitalWrite(LED_BUILTIN, HIGH);

                // Activate corresponding function
                switch(specialKey.function) 
                {
                    case cycleKeyMap: 
                        CycleKeyMap();
                        break;
                    case useDefaultKeyMap:
                        // TODO: Create solution for a default keymap.
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