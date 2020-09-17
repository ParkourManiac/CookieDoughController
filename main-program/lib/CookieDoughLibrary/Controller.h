#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "Key.h"
#include "LinkedList.h"
#include "LinkedList.cpp" // NOLINT(build/include)
#include "EditMode.h"
#include "DataPacket.h"

class Controller
{
private:
    int normalKeyCount;
    BareKeyboardKey *defaultKeymap;
    
    int specialKeyCount;
    SpecialKey *specialKeys;

    /**
     * @brief Is true when the default keymap is equipped.
     */
    bool isUsingDefaultKeymap = false;

    Key *currentKeyMap;

    unsigned int customKeyMapIndex = 0;
    LinkedList<BareKeyboardKey *> *customKeyMapsPtr;
    LinkedList<BareKeyboardKey *> customKeyMaps;

    const uint8_t bufferSize = 8;
    uint8_t *buf; // Keyboard report buffer.

    unsigned int eepromAdress = 0;
    unsigned int nextFreeEepromAdress = 0;

    EditMode editmode = EditMode(normalKeyCount, true);

    const float longPressDuration = 2000;

public:
    /**
     * @brief Construct a new Controller object and sets up the Controllers initial keymap and the keymap used for special functions.
     * 
     * @param _defaultKeymap The keymap to be used as the default keymap.
     * @param amountOfDefaultKeys The amount of keys inside the default keymap.
     * @param _specialKeys The keymap to be used for special function keys.
     * @param amountOfSpecialKeys The amount of keys inside the specialKeys keymap.
     */
    Controller(BareKeyboardKey *_defaultKeymap, int amountOfDefaultKeys, SpecialKey *_specialKeys, int amountOfSpecialKeys);
    Controller(const Controller& other);
    ~Controller();
    void operator=(const Controller&) = delete;

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
    void SaveKeyMapsToMemory(LinkedList<BareKeyboardKey *> keyMapList);

    /**
     * @brief If present on the EEPROM, Loads a list of keymaps from memory
     * into the provided keyMap list.
     *
     * @param keyMapList The keyMap list to store the result.
     */
    void LoadKeymapsFromMemoryIntoList(LinkedList<BareKeyboardKey *> *keyMapList); // NOTE: Refactored to BareKeyboardKeys

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
    bool RetrieveBareKeyboardKeysFromMemory(BareKeyboardKey **payloadAsBareKeys, unsigned int *amountOfKeys, unsigned int *packetAdress, unsigned int *packetSize);

    /**
     * @brief Finds and retrieves a DataPacket stored on the EEPROM.
     * 
     * @param packet The DataPacket found on the EEPROM.
     * @param packetAdress The EEPROM adress where the DataPacket was found.
     * @param packetSize The size of the DataPacket.
     * @return true The DataPacket was succesfully found and retrieved.
     * @return false The DataPacket was not found or the DataPackets found were corrupt.
     */
    bool RetrieveDataPacketFromMemory(DataPacket *packet, unsigned int *packetSize, unsigned int *packetAdress, unsigned int startAdress = 0);

    /**
     * @brief Converts a DataPacket payload into an array of BareKeyboardKeys.
     * 
     * @param packet The packet to be processed.
     * @param result A pointer to the resulting array of BareKeyboardKeys.
     */
    void ConvertDataPacketToBareKeyboardKeys(DataPacket packet, BareKeyboardKey *result);

    /**
     * @brief Populates the provided keymap list using the BareKeyboardKey array.
     * 
     * @param keys The array of keys to be inserted into the list.
     * @param amountOfKeys The amount of keys in the "keys" array.
     * @param keymapList The list in which the keys will be inserted.
     */
    void ParseBareKeyboardKeyArrayIntoKeymapList(BareKeyboardKey *keys, unsigned int amountOfKeys, LinkedList<BareKeyboardKey *> *keymapList);

    /**
     * @brief Determines whether a pin is valid (i.e can be used) or not.
     * 
     * @param pin The pin to be validated.
     * @return true The pin is valid.
     * @return false The pin is invalid or corrupt.
     */
    bool IsKeyValid(const IKey &pin);

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
     * @brief Equips the current keymap with the provided keymaps configuration.
     * 
     * @param keyMap A pointer to the keymap we want to equip.
     */
    void ChangeKeyMap(BareKeyboardKey *keyMap);

    /**
     * @brief Updates the equipped custom keymap with the pin and keycode from the currentKeyMap.
     */
    void UpdateCurrentCustomKeymap();
    
    /**
     * @brief Switches to the built in default keyMap.
     */
    void ToggleDefaultKeyMap();

    /**
     * @brief Writes the keypress events to the buffer and sends them to the computer.
     */
    void SendKeyInfo();

    /**
     * @brief Sets the buffer to only zeroes. Thus, removing all keyboard events from the buffer.
     */
    void WipeKeyboardEventBuffer();

    /**
     * @brief Transmits the buffer data to the connected device through the serial port.
     */
    void SendKeyboardEvent();

    /**
     * @brief Executes the corresponding special function when a special key is pressed.
     */
    void ExecuteSpecialCommands();

    /**
     * @brief Toggles into or out of editmode for the controller.
     * Note: If no custom keymap exist when entering editmode, a keymap will be created automatically.
     */
    void ToggleEditMode();

    /**
     * @brief Saves the current setup of custom keymaps to memory.
     * Note: Will wait for the EEPROM to finish writing before continuing.
     */
    void SaveControllerSettings();

    /**
     * @brief Deletes the currently selected keymap. 
     * Note: Can only delete custom keymaps. Cannot be used on default keymap.
     */
    void DeleteCurrentKeyMap();

    /**
     * @brief Creates a new keymap and adds it to the list of custom keymaps.
     * 
     * @return true The keymap was successfully created.
     * @return false The keymap couldn't be created due to insufficient amount of free space or an error occured.
     */
    bool CreateNewKeyMap();

    /**
     * @brief Quickly blinks the builtin led 5 times to indicate that something went wrong.
     */
    void SignalErrorToUser();
};

#endif