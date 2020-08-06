#ifndef NORMAL_KEY_COUNT 
#define NORMAL_KEY_COUNT 4 // Move controller into class and set these as a class variable instead
#endif
#ifndef SPECIAL_KEY_COUNT
#define SPECIAL_KEY_COUNT 3 // Move controller into class and set these as a class variable instead
#endif

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include <Key.h>
#include <LinkedList.h>
#include <LinkedList.cpp>
#include <EditMode.h>

extern Key defaultKeyMap[NORMAL_KEY_COUNT];
extern SpecialKey specialKeys[SPECIAL_KEY_COUNT];

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

void ToggleEditMode();

void SaveControllerSettings();
void DeleteCurrentKeyMap();
bool CreateNewKeyMap();
void SignalErrorToUser();


// Miscellaneous:
// Key setup WASD
// Key keys[4] = {
// {.pin = 2, .keyCode = 4},
// {.pin = 3, .keyCode = 26},
// {.pin = 4, .keyCode = 22},
// {.pin = 5, .keyCode = 7},
// };

// Key setup Arrow keys
// Key keys[4] = {
//     {.pin = 2, .keyCode = 80},
//     {.pin = 3, .keyCode = 82},
//     {.pin = 4, .keyCode = 81},
//     {.pin = 5, .keyCode = 79},
// };

#endif