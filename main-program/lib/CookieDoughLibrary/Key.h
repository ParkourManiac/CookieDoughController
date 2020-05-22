#ifndef KEY_H
#define KEY_H

/**
 * @brief Used to setup a pin to behave as a keyboard key.
 */
struct Key {
    int pin;
    int keyCode;
    bool value;
    bool oldValue;
    unsigned long lastDebounceTime;
    bool oldPinState;
};

/**
 * @brief Used to specify the functionality of a SpecialKey.
 */
enum SpecialFunction {
    toggleEditMode,  /**< Toggle between edit (keymap) mode and controller mode. */
    cycleKeyMap,  /**< Cycle between available key maps. */
    toggleDefaultKeyMap /**< Toggle between the default and custom key map. */
};

/**
 * @brief Used to setup a pin to behave as a key with special functionality.
 */
struct SpecialKey {
    int pin;
    SpecialFunction function;
    bool value;
    bool oldValue;
};

#endif