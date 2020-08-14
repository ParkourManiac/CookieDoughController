#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "Key.h"
#include "LinkedList.h"
#include "LinkedList.cpp"
#include "EditMode.h"
#include "DataPacket.h"

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
     * @brief Construct a new Controller object and sets up the Controllers initial keymap and the keymap used for special functions.
     * 
     * @param _defaultKeyMap The keymap to be used as the default keymap.
     * @param amountOfDefaultKeys The amount of keys inside the default keymap.
     * @param _specialKeys The keymap to be used for special function keys.
     * @param amountOfSpecialKey The amount of keys inside the specialKeys keymap.
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
     * @brief If present on the EEPROM, Loads a list of keymaps from memory
     * into the provided keyMap list.
     *
     * @param keyMapList The keyMap list to store the result.
     */
    void LoadKeymapsFromMemoryIntoList(LinkedList<Key *> &keyMapList);

    /**
     * @brief Retrieves the saved BareKeyboardKeys stored in the EEPROM.
     * WARNING, payloadAsBareKeys MUST BE ALLOCATED ON THE HEAP (IF NOT THE PROGRAM WILL CRASH).
     * 
     * @param payloadAsBareKeys A pointer to the output array of retrieved keys. 
     * @param amountOfKeys The amount of keys retrieved.
     * @param packetAdress The EEPROM location of the packet.
     * @param packetSize The size of the packet.
     * @return true If we succesfully retrieved the array of keys.
     * @return false If we were unable to find any valid packet.
     */
    bool RetrieveBareKeyboardKeysFromMemory(BareKeyboardKey *&payloadAsBareKeys, unsigned int &amountOfKeys, unsigned int &packetAdress, unsigned int &packetSize);

    /**
     * @brief Finds and retrieves a DataPacket stored on the EEPROM.
     * 
     * @param packet The DataPacket found on the EEPROM.
     * @param packetAdress The EEPROM adress where the DataPacket was found.
     * @param packetSize The size of the DataPacket.
     * @return true The DataPacket was succesfully found and retrieved.
     * @return false The DataPacket was not found or the DataPackets found were corrupt.
     */
    bool RetrieveDataPacketFromMemory(DataPacket &packet, unsigned int &packetSize, unsigned int &packetAdress, unsigned int startAdress = 0);

    /**
     * @brief Converts a DataPacket payload into an array of BareKeyboardKeys.
     * 
     * @param packet The packet to be processed.
     * @param result A pointer to the resulting array of BareKeyboardKeys.
     */
    void ConvertDataPacketToBareKeyboardKeys(DataPacket packet, BareKeyboardKey *result);

    /**
     * @brief Populates the provided keymap list using the BareKeyboardKeys.
     * 
     * @param keys The keys to be inserted into the list.
     * @param amountOfKeys The amount of keys to be inserted.
     * @param keymapList The list in which the keys will be inserted.
     */
    void ParseBareKeyboardKeysIntoKeymapList(BareKeyboardKey *keys, unsigned int amountOfKeys, LinkedList<Key *> &keymapList);

    /**
     * @brief Determines whether a key is valid (i.e can be used) or not.
     * 
     * @param key The key to be validated.
     * @return true The key is valid.
     * @return false The key is invalid or corrupt.
     */
    bool IsKeyValid(IKey key);

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