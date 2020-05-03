#include <Arduino.h>
#include <Key.h>


// HEADER
void ConfigurePinsAsKeys();
void CycleKeyMap();
void ChangeKeyMap(int index);
void ReadPinValueForKeys();
void SendKeyInfo();
void ExecuteSpecialCommands();

// Public variables
Key defaultKeyMap[4] = { // Key map WASD
    {.pin = 2, .keyCode = 80},
    {.pin = 3, .keyCode = 82},
    {.pin = 4, .keyCode = 81},
    {.pin = 5, .keyCode = 79},
};

Key* currentKeyMap = &defaultKeyMap[0]; // TODO: Continue converting things to pointers.

Key* availableKeyMaps;

Key keyMaps[2][4] = {
    {   // Key map WASD
        {.pin = 2, .keyCode = 80},
        {.pin = 3, .keyCode = 82},
        {.pin = 4, .keyCode = 81},
        {.pin = 5, .keyCode = 79},
    },
    { // Key map Arrow keys
        {.pin = 2, .keyCode = 4},
        {.pin = 3, .keyCode = 26},
        {.pin = 4, .keyCode = 22},
        {.pin = 5, .keyCode = 7},
    }
};

SpecialKey specialKeys[1] = { // Should never change.
    {.pin = 12, .function = cycleKeyMap}
};

uint8_t buf[8] = {0}; // Keyboard report buffer.

// PROGRAM
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    ConfigurePinsAsKeys();
}

void loop()
{
    ReadPinValueForKeys();
    ExecuteSpecialCommands();
    SendKeyInfo();
}

/**
 * @brief Configures pins marked as Key or SpecialKey to act as input pins with internal pullups.
 */
void ConfigurePinsAsKeys() {
    for(Key& key : keyMaps[currentKeyMapIndex]) {
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
    ChangeKeyMap(currentKeyMapIndex + 1);
}

/**
 * @brief Changes the current keymap to the keymap specified at the given index (in the available keyMaps).
 * 
 * @param index The index of the keymap to be switched to.
 */
void ChangeKeyMap(int index) {
    int length = sizeof(keyMaps) / sizeof(keyMaps[0]);
    currentKeyMapIndex = index % length;

    ConfigurePinsAsKeys();
}

/**
 * @brief Reads and updates the pin values of
 *  the current keymap and special keys.
 */
void ReadPinValueForKeys()
{
    for(Key& key : keyMaps[currentKeyMapIndex]) {
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
    for(Key& key : keyMaps[currentKeyMapIndex]) {
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