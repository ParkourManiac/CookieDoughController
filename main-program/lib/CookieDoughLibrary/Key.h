#ifndef KEY_H
#define KEY_H

#include <stdint.h>

/**
 * @brief Used to marks a pin as a key.
 * Contains the pin number.
 */
struct IKey
{
    uint8_t pin; /** The pin number. */
};

/**
 * @brief Contains the pin, pin state and debounce values of the pin.
 */
struct IPinState : virtual IKey
{
    bool value = false;                 /** The value of the pin. true = active, false = inactive. */
    bool oldValue = false;              /** The previous value of the pin. */
    unsigned long timeOfActivation = 0; /** The time of the last activation. */
    unsigned long lastDebounceTime = 0; /** The time, in milliseconds, of the latest change in pin state. */
    bool oldPinState = false;           /** The previous pin state. */
};

/**
 * @brief The bare minimum to define a pin as a keyboard key.
 * Note: Contains no state.
 */
struct BareKeyboardKey : virtual IKey
{
    int keyCode; /** The keyboard keycode. */
    BareKeyboardKey() {}
    BareKeyboardKey(uint8_t _pin, int _keyCode) // TODO: Test if working.
    {
        pin = _pin;
        keyCode = _keyCode;
    }
};

/**
 * @brief Used to set up a pin as a keyboard key and store its state.
 * Contains both the definition of the keyboard key and 
 * the state of the corresponding pin.
 */
struct Key : virtual BareKeyboardKey, virtual IPinState
{
    Key() {}
    Key(uint8_t _pin, int _keyCode)
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
struct SpecialKey : virtual IPinState
{
    SpecialFunction function; /**< The special function tied to the key. */

    SpecialKey(uint8_t _pin, SpecialFunction _function)
    {
        pin = _pin;
        function = _function;
    }
};

/**
 * @brief Configures the pin of the provided key
 * to act as an input pin with internal pullup.
 * 
 */
void ConfigurePinForKey(IKey &key);

/**
 * @brief Will try to convert the given type into IKey and 
 * configures the pins of the provided keymap
 * to act as input pins with internal pullups.
 * 
 * @tparam T The type of key to be used. 
 * NOTE: Must inherit from the base class IKey.
 * @param keyMap The keymap to be configured.
 * @param keyMapLength The length of the keyMap.
 */
template <class T>
void ConfigurePinsForKeyMap(T *keyMap, unsigned int keyMapLength)
{
    for (unsigned int i = 0; i < keyMapLength; i++)
    {
        ConfigurePinForKey((IKey &)keyMap[i]);
    }
}

/**
 * @brief Reads and updates the pin state of a
 * key using a debounced input.
 * 
 * @param key The key to be updated.
 */
void DebounceRead(IPinState &key);

/**
 * @brief Reads and updates the pin state of
 * the provided keyMap.
 */
template <class T>
void ReadPinValuesForKeyMap(T *keyMap, unsigned int keyMapLength)
{
    for (unsigned int i = 0; i < keyMapLength; i++)
    {
        DebounceRead((IPinState &)keyMap[i]);
    }
}

/**
 * @brief Checks if the key was just pressed.
 * 
 * @param key The key to be analysed.
 * @return true If the key was pressed.
 * @return false If the key wasn't pressed.
 */
bool OnKeyPress(IPinState &key);

/**
 * @brief Checks if the key was just released.
 * 
 * @param key The key to be analysed.
 * @return true If the key was released.
 * @return false If the key wasn't released.
 */
bool OnKeyRelease(IPinState &key);

/**
 * @brief Checks if the time since the keys activation is greater than a long press duration.
 * NOTE: This does not take into account that the button may be released.
 * 
 * @param key The key to be analysed.
 * @param longPressDuration The duration for a long press.
 * @return true If the key was activated more than longPressDuration ago, it will return true.
 * @return false If the key was activated less than longPressDuration ago, it will return false.
 */
bool OnLongPress(IPinState key, unsigned int longPressDuration);

#endif