#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "Key.h"
#include "LinkedList.h"
#include "LinkedList.cpp"
#include "EditMode.h"

class Controller
{
private:
    Key *defaultKeyMap;
    int normalKeyCount;
    
    SpecialKey *specialKeys;
    int specialKeyCount;

    Key *currentKeyMap = defaultKeyMap;
    unsigned int customKeyMapIndex = 0;

    LinkedList<Key *> *customKeyMapsPtr = new LinkedList<Key *>();
    LinkedList<Key *> customKeyMaps = *customKeyMapsPtr;

    uint8_t buf[8] ={ 0 }; // Keyboard report buffer.

    unsigned int eepromAdress = 0;
    unsigned int nextFreeEepromAdress = 0;

    EditMode editmode = EditMode(true);

    const float longPressDuration = 4000;

public:
    /**
     * @brief Sets up the controllers initial keymap and the keymap used for special functions.
     *
     */
    Controller(Key *_defaultKeyMap, int amountOfDefaultKeys, SpecialKey *_specialKeys, int amountOfSpecialKey) 
        : defaultKeyMap(_defaultKeyMap)
        , normalKeyCount(amountOfDefaultKeys)
        , specialKeys(_specialKeys)
        , specialKeyCount(amountOfSpecialKey)
    {
    }

    /**
     * @brief Sets up the controller by loading the keymaps from memory and configuring the corresponding Arduino pin for each key.
     */
    void Setup();

    /**
     * @brief The update loop of the controller. Handles everything from updating state to sending the keystrokes.
     */
    void Update();

    /**
     * @brief Saves a list of keymaps to memory.
     * Note: Will overwrite existing keymaps on the eeprom.
     *
     * @param keyMapList The list of keymaps to be saved.
     */
    void SaveKeyMapsToMemory(LinkedList<Key *> keyMapList);

    /**
     * @brief Loads the list of keymaps from memory
     * into the provided keyMap list.
     *
     * @param keyMapList The keyMap list to store the result.
     */
    void LoadKeyMapsFromMemory(LinkedList<Key *> &keyMapList);

    /**
     * @brief Switches to the next keyMap configuration in the list
     * of available keyMaps.
     * Note: If we are using the default keyMap then it will
     * switch back to the previous keyMap instead of moving
     * to the next keyMap in the list.
     *
     */
    void CycleKeyMap();

    /**
     * @brief Changes the current keymap to the keymap specified at
     * the given index (in the available keyMaps).
     *
     * @param index The index of the keymap to be switched to.
     */
    void ChangeKeyMap(Key *keyMap);
    /**
     * @brief Switches to the built in default keyMap.
     */
    void ToggleDefaultKeyMap();

    /**
     * @brief Writes the keypress events to the buffer and sends them to the computer.
     */
    void SendKeyInfo();

    /**
     * @brief Executes the corresponding special function when a special key is pressed.
     */
    void ExecuteSpecialCommands();

    // TODO: Document all functions
    void ToggleEditMode();

    void SaveControllerSettings();
    void DeleteCurrentKeyMap();
    bool CreateNewKeyMap();
    void SignalErrorToUser();
};

#endif