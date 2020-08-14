#include <Controller.h>

const int normalKeyCount = 4;
Key defaultKeyMap[normalKeyCount] = {
    // Key map Arrow keys
    Key(2, 80),
    Key(3, 82),
    Key(4, 81),
    Key(5, 79),
};

const int specialKeyCount = 3;
SpecialKey specialKeys[specialKeyCount] = {
    SpecialKey(10, toggleEditMode),
    SpecialKey(11, cycleKeyMap),
    SpecialKey(12, toggleDefaultKeyMap), // This one should never change.
};

// Miscellaneous:
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



Controller controller(defaultKeyMap, normalKeyCount, specialKeys, specialKeyCount);