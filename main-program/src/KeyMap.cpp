#include <Key.h>
class KeyMap {
    public:
        Key* keyMap;
        int length;

        KeyMap(Key keys[]) { // Constructor
            length = sizeof(keys) / sizeof(keys[0]);
            keyMap = keys;
        }
    /*
        * Pointer to list.
        * int for length.
        * Constructor to set the size automatically.
        - Should we use Key[] so that it can be stored?
    */  
};

class KeyMapList {

    /*
        pointer to list.
        int for size.
        function to add keymaps to list and realloc.
        function to remove keymaps from list and realloc.
        function to get a specific keymap. Loop through sizes and calculate position.
    */
};