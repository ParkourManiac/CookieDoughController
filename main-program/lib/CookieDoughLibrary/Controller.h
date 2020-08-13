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
    void LoadKeymapsFromMemoryIntoList(LinkedList<Key *> &keyMapList);

    /**
     * @brief Retrieves the saved BareKeyboardKeys stored in the EEPROM.
     * 
     * @param payloadAsBareKeys A pointer to an array of the retrieved keys.
     * @param amountOfKeys The amount of keys retrieved
     * @param packetAdress The location of the packet from which the keys were retrieved.
     * @param packetSize The size of the packet from which the keys were retrieved.
     * @return true If we succesfully retrieved the array of keys.
     * @return false If we were unable to find any valid packet.
     */
    bool RetrieveBareKeyboardKeysFromMemory(BareKeyboardKey *payloadAsBareKeys, unsigned int &amountOfKeys, unsigned int &packetAdress, unsigned int &packetSize);

    /**
     * @brief Finds and retrieves a DataPacket stored on the EEPROM.
     * 
     * @param packet The DataPacket found on the EEPROM.
     * @param packetAdress The adress where the DataPacket was found.
     * @param packetSize The size of the DataPacket.
     * @return true The DataPacket was succesfully found and retrieved.
     * @return false The DataPacket was not found or the DataPackets found were corrupt.
     */
    bool RetrieveDataPacketFromMemory(DataPacket &packet, unsigned int &packetSize, unsigned int &packetAdress);

    /**
     * @brief Converts a DataPacket payload into an array of BareKeyboardKeys.
     * 
     * @param packet The packet to be processed.
     * @param result The payload as an array of BareKeyboardKeys.
     * @param amountOfKeys The amount of keys in the payload.
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