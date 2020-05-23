#ifndef KEY_H
#define KEY_H

/**
 * @brief Used to marks a pin as a key.
 * Contains the pin number.
 */
struct IKey
{
    int pin; /** The pin number. */
};

/**
 * @brief Contains the pin state and debounce values of the pin.
 */
struct IPinState : virtual IKey
{
    bool value = false;                 /** The value of the pin. true = active, false = inactive. */
    bool oldValue = false;              /** The previous value of the pin. */
    unsigned long lastDebounceTime = 0; /** The time, in milliseconds, of the latest change in pin state. */
    bool oldPinState = false;           /** The previous pin state. */
};


/**
 * @brief The definition of a pin as a keyboard key.
 * Note: Contains no state. Is used as an alternative to
 */
struct BareKeyboardKey : virtual IKey
{
    int keyCode; /** The keyboard keycode. */
};

/**
 * @brief Used to set up a pin as a keyboard key and store its state.
 * Contains both the definition of the keyboard key and 
 * the state of the corresponding pin.
 */
struct Key : virtual BareKeyboardKey, virtual IPinState
{
    Key() {}
    Key(int _pin, int _keyCode)
    {
        pin = _pin;
        keyCode = _keyCode;
    }
};

/**
 * @brief Used to specify the functionality of a SpecialKey.
 */
enum SpecialFunction
{
    toggleEditMode,     /**< Toggle between edit (keymap) mode and controller mode. */
    cycleKeyMap,        /**< Cycle between available key maps. */
    toggleDefaultKeyMap /**< Toggle between the default and custom key map. */
};

/**
 * @brief Used to setup a pin to behave as a key with special functionality.
 * Contains both the definition of the special function and 
 * the state of the corresponding pin.
 */
struct SpecialKey : virtual IPinState, virtual IKey
{
    SpecialFunction function; /**< The special function tied to the key. */

    SpecialKey(int _pin, SpecialFunction _function)
    {
        pin = _pin;
        function = _function;
    }
};

#endif