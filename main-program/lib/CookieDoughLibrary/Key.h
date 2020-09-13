#ifndef KEY_H
#define KEY_H

#include <stdint.h>

/**
 * @brief An alias for the type of a pin number.
 */
using IKey = uint8_t;

/**
 * @brief An alias for the type of a keycode.
 * 
 */
using IKeycode = int;

/**
 * @brief Used to store information about a pins state and debounce values.
 */
struct IPinState
{
    bool value = false;                 /** The value of the pin. true = active, false = inactive. */
    bool oldValue = false;              /** The previous value of the pin. */
    uint32_t timeOfActivation = 0;      /** The time of the last activation. */
    uint32_t lastDebounceTime = 0;      /** The time, in milliseconds, of the latest change in pin state. */
    bool oldPinState = false;           /** The previous pin state. */
};

/**
 * @brief The bare minimum to define a pin as a keyboard key.
 * Note: Contains no state.
 */
struct /*__attribute__((packed))*/ BareKeyboardKey // TODO: Refactor to not use constructor (or use compact constructor)? Consider using packed.
{
    IKey pin; /** The pin connected to the key. */
    IKeycode keyCode; /** The keyboard keycode. */
    BareKeyboardKey() {}
    BareKeyboardKey(IKey _pin, IKeycode _keyCode) // TODO: Test if working.
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
struct Key // TODO: Refactor to not use constructor (or use compact constructor)?
{
    IKey pin; /** The pin connected to the key. */
    IKeycode keyCode; /** The keyboard keycode. */
    IPinState state; /** Contains information about the state of the button. */
    Key() {}
    Key(IKey _pin, IKeycode _keyCode)
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
struct SpecialKey // TODO: Refactor to not use constructor (or use compact constructor)?
{
    IKey pin; /** The pin connected to the key. */
    SpecialFunction function; /** The special function tied to the key. */
    IPinState state; /** Contains information about the state of the button. */

    SpecialKey(IKey _pin, SpecialFunction _function)
    {
        pin = _pin;
        function = _function;
    }
};

/**
 * @brief Configures a pin
 * to act as an input pin with internal pullup.
 * 
 * @param pin The number of the pin to be configured.
 */
void ConfigurePinForKey(const IKey &pin);

// /**
//  * @brief Will try to convert the given type into IKey and 
//  * configures the pins of the provided keymap
//  * to act as input pins with internal pullups.
//  * 
//  * @tparam T The type of key to be used. 
//  * NOTE: Must inherit from the base class IKey.
//  * @param keyMap The keymap to be configured.
//  * @param keyMapLength The length of the keyMap.
//  */
// template <class T>
// void ConfigurePinsForKeyMap(T *keyMap, unsigned int keyMapLength) // TODO: Remove? Is not compatible with refactored IKey.
// {
//     for (unsigned int i = 0; i < keyMapLength; i++)
//     {
//         ConfigurePinForKey((IKey &)keyMap[i]);
//     }
// }

/**
 * @brief Will configure all the pins of the provided keymap
 * to act as input pins with internal pullups.
 * 
 * @param keymap The keymap to be configured.
 * @param keymapLength The length of the keymap.
 */
template <class T>
void ConfigurePinsForKeyMap(const T *keymap, unsigned int keymapLength) 
{
    for (unsigned int i = 0; i < keymapLength; i++)
    {
        ConfigurePinForKey(keymap[i].pin);
    }
}

/**
 * @brief Reads and updates the given state of a
 * pin using a debounced input.
 * 
 * @param pin The pin of the key
 * @param state The state to be updated.
 */
void DebounceRead(IKey pin, IPinState *state); // TODO: Refactor name to "DebounceReadPin"? 

/**
 * @brief Reads and updates the pin state of
 * the provided keyMap.
 */
template <class T>
void UpdatePinStatesForKeyMap(T *keymap, unsigned int keymapLength) // TODO: Remove? Is not compatible with refactored IKey.
{
    for (unsigned int i = 0; i < keymapLength; i++)
    {
        DebounceRead(keymap[i].pin, &(keymap[i].state));
    }
}

/**
 * @brief Validates if the state of a key is a key press event.
 * 
 * @param state The state to be analysed.
 * @return true If the state indicates a key press.
 * @return false If the state does not indicate a key press.
 */
bool OnKeyPress(const IPinState &state);

/**
 * @brief Validates if the state of a key is a key release event.
 * 
 * @param state The state to be analysed.
 * @return true If the state indicates a key release.
 * @return false If the state does not indicates a key release.
 */
bool OnKeyRelease(const IPinState &state);

/**
 * @brief Validates that the key has been held down for a time greater than a long press duration.
 * NOTE: This does not take into account that the button may currently be released.
 * 
 * @param state The state to be analysed.
 * @param longPressDuration The duration for a long press.
 * @return true If the state has been active for longer than a longPressDuration, it will return true.
 * @return false If the state has been active for less than a longPressDuration, it will return false.
 */
bool OnLongPress(const IPinState &state, unsigned int longPressDuration);

#endif