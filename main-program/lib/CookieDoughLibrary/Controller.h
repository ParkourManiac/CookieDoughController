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
    const int normalKeyCount;
    BareKeyboardKey *defaultKeymap;
    
    const int specialKeyCount;
    SpecialKey *specialKeys;

    /**
     * @brief Is true when the default keymap is equipped.
     */
    bool isUsingDefaultKeymap = false;

    Key *currentKeyMap;

    unsigned int customKeyMapIndex = 0;
    LinkedList<BareKeyboardKey *> *customKeyMapsPtr;
    LinkedList<BareKeyboardKey *> customKeyMaps; // TODO: Consider making into a reference variable?

    const uint8_t bufferSize = 8;
    uint8_t *buf; // Keyboard report buffer.

    const uint16_t storageSize;
    
    /**
     * @brief Represents the amount of free storage space available for use.
     */
    uint16_t amountOfFreeStorage;
    int32_t SRAMSafetyThreshold = 500;

    uint16_t currentPacketAdress = 0;
    uint16_t nextPacketAdress = 0;

    EditMode editmode = EditMode(normalKeyCount, true);

    const unsigned long longPressDuration = 2000;

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
     * @brief Prevents the provided adress from exceeding the last adress of the EEPROM,
     * and converts the adress into a cyclic format. 
     * 
     * @param adress The adress to be made cyclic.
     * @return uint16_t Returns the adress in a safe cyclic format. If the adress exceeds
     * the last adress of the EEPROM, a new safe adress that wraps back to the start of the EEPROM
     * will be returned. 
     * If the adress does not exceed the EEPROMs last adress, the returned adress 
     * will be equal to the provided adress.
     */
    uint16_t CyclicEepromAdress(uint32_t adress);

    /**
     * @brief Returns the amount of unused storage in bytes if we were to 
     * save the given number of keymaps to the storage unit.
     * 
     * @param amountOfKeymaps The amount of keymaps we want to assume to 
     * be saved to the storage unit. 
     * If left as default (0), the function will use the amount of customKeyMaps
     * on the controllers to calculate the unused storage.
     * @return uint16_t Returns the amount of unused storage in bytes for the 
     * scenario when we save the given number of keymaps to the storage unit.
     * If the returned amount is negative, the absolute value of the result
     * will represent the amount of overflowing bytes that will not fit into
     * the storage unit.
     */
    int32_t CalculateUnusedStorage(uint16_t amountOfKeymaps = 0);

    /**
     * @brief Saves a list of keymaps to memory.
     * Note: Will overwrite any data already present on the eeprom.
     *
     * @param keyMapList The list of keymaps to be saved.
     * @return true The keymaps were successfully saved to the EEPROM.
     * @return false Failed to save the keymaps.
     */
    bool SaveKeyMapsToMemory(const LinkedList<BareKeyboardKey *> &keymapList);

    /**
     * @brief Loads available keymaps from storage into the provided list of
     * keymaps.
     *
     * @param keymapList The list of keymaps to where the result will be 
     * concatinated.
     * @return true The keymaps found in storage has been loaded into the
     * provided keymap list.
     * @return false Could not find or load any valid keymaps from memory.
     */
    bool LoadKeymapsFromMemoryIntoListV2(LinkedList<BareKeyboardKey *> *keymapList);

    /**
     * @brief Parse the given payload as a list of keymaps and concatinate the
     * result into the provided keymap list.
     * 
     * @param payloadAdress The adress of the payload to be parsed.
     * @param payloadLength The length of the payload.
     * @param keymapList The list where the resulting keymaps will be 
     * concatinated into.
     * @return true Successfully parsed and concatinated the keymaps into the 
     * given keymap list.
     * @return false Either failed to read the bytes from storage or the 
     * payload does not contain a valid array of keymaps.
     */
    bool AddKeymapsFromPayloadIntoList(const uint16_t &payloadAdress, const uint16_t &payloadLength, LinkedList<BareKeyboardKey *> *keymapList);


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
    bool RetrieveBareKeyboardKeysFromMemory(BareKeyboardKey **payloadAsBareKeys, uint16_t *amountOfKeys, uint16_t *packetAdress, uint16_t *packetSize);

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
    bool CreateNewKeymap();

    /**
     * @brief Quickly blinks the builtin led 5 times to indicate that something went wrong.
     */
    void SignalErrorToUser();
};

#endif