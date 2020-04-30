/**
 * @brief Used to setup a pin to behave as a keyboard key.
 */
struct Key {
    int pin;
    int keyCode;
    bool value;
    bool oldValue;
};

/**
 * @brief Used to specify the functionality of a SpecialKey.
 */
enum SpecialFunction {
    cycleKeyMap,  /**< Cycle between available key maps. */
    useDefaultKeyMap /**< Switch to the default key map. */
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