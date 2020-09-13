#ifndef EDIT_MODE_H
#define EDIT_MODE_H

#include "Key.h"

class EditMode {
public:
    /**
     * @brief Returns true if editmode is currently turned on.
     */
    bool enabled = false;
    /**
     * @brief If true, editmode will continously blink a signal
     * on the BUILTIN_LED as long as editmode is turned on (and
     * no keys are currently being pressed).
     */
    bool useEditModeLedSignal;

private:
    const int normalKeyCount;
    Key *selectedKey = nullptr;
    Key *tempCopy = nullptr;
    int keysPressed = 0;
    int inputKeyCode = 0;
    bool shouldAddValue = false;

    const int blinksPerSignal = 3;
    bool ledIsOn = false;
    uint32_t nextBlinkCycle = 0;
    uint32_t nextBlinkCycleOff = 0;
    int currentBlink = 0;

public:
    explicit EditMode(int _normalKeyCount, bool _useEditModeLedSignal);
    ~EditMode();

    /**
     * @brief Toggles and resets EditMode.
     */
    void Toggle();

    /**
     * @brief Copies the provided keyMap into the temporary placeholder.
     * 
     * @param keyMap The keyMap from which the information will be copied.
     */
    void CopyKeyMapToTemporary(Key *keyMap);

    /**
     * @brief Overwrites the provided keymap with the values in the temporary placeholder.
     * 
     * @param keyMapToRestore The keyMap to be overwritten with the values of the temporary placeholder.
     */
    void RestoreKeyMapFromTemporaryCopy(Key *keyMapToRestore);

    /**
     * @brief Handles the key press and key release events of the
     * provided keymap and edits the provided keymap accordingly. 
     * NOTE: Put this in your loop after the keyMap values has 
     * been updated by the user input.
     * 
     * @param keyMapBeingEdited The keyMap to edit.
     */
    void EditModeLoop(Key* keyMapBeingEdited);

private:
    /**
     * @brief Handles selecting which key to edit and
     * records keyPress events for editing the keyCode. 
     * 
     * @param pressedKey The key that was pressed.
     */
    void RegisterKeyPress(Key *pressedKey);

    /**
     * @brief Handles calculating and updating the
     * keycode of the selected key.
     */
    void RegisterKeyRelease();

    /**
     * @brief Signals to the user (by blinking 3 times
     * every second on LED 13) that the user currently
     * is in editmode.
     * 
     */
    void SignalLedEditMode();

    /**
     * @brief Resets all temporary values of editmode
     * to their initial value.
     */
    void Reset();
};



#endif