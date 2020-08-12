#include "test.h"
#include "../lib/CookieDoughLibrary/Controller.h"
#include "../lib/CookieDoughLibrary/DataPacket.h"
#include "../lib/CookieDoughLibrary/LinkedList.h"
#include "../lib/CookieDoughLibrary/DataPacket.h"
#include "Fakes/Arduino.h"
#include "Fakes/EEPROM.h"
#include "TestHelper/DataPacketHelper.h"

extern std::vector<uint8_t> EEPROMClass_read_return_v;

extern std::vector<uint16_t> EEPROMClass_length_return_v;

Controller SetUpController()
{
    const int normalKeyCount = 4;
    Key defaultKeyMap[normalKeyCount] ={
        // Key map Arrow keys
        Key(2, 80),
        Key(3, 82),
        Key(4, 81),
        Key(5, 79),
    };

    const int specialKeyCount = 3;
    SpecialKey specialKeys[specialKeyCount] ={
        SpecialKey(10, toggleEditMode),
        SpecialKey(11, cycleKeyMap),
        SpecialKey(12, toggleDefaultKeyMap),
    };

    return Controller(defaultKeyMap, normalKeyCount, specialKeys, specialKeyCount);
}

void RetrieveDataPacketFromMemory_DataPacketIsPresentOnEEPROM_RetrievesTheDataPacketAndReturnsTrue()
{
    Controller controller = SetUpController();
    // Makes the function fail to find a packet on the first adress.
    EEPROMClass_read_return_v.push_back(0);
    EEPROMClass_length_return_v.push_back(1000);
    EEPROMClass_length_return_v.push_back(1000);
    // Makes the function fail to find a packet on the second adress.    
    EEPROMClass_read_return_v.push_back(0);
    EEPROMClass_length_return_v.push_back(1000);
    EEPROMClass_length_return_v.push_back(1000);
    // Makes the function find a packet on the third adress. (Index 2 in EEPROM)
    uint16_t data = 1337;
    uint8_t *dataPtr = (uint8_t*)&data;
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);


    DataPacket *resultPtr = new DataPacket();
    DataPacket result = *resultPtr;
    unsigned int packetSize;
    unsigned int packetAdress;
    bool resultBool = controller.RetrieveDataPacketFromMemory(result, packetSize, packetAdress);

    ASSERT_TEST(
        resultBool == true &&
        packet.stx == result.stx &&
        packet.payloadLength == result.payloadLength &&
        packet.crc == result.crc &&
        *((uint16_t*)packet.payload) == data &&
        packet.etx == result.etx &&
        packetAdress == 2 &&
        packetSize == 10
    );

    delete(resultPtr);
}

void RetrieveDataPacketFromMemory_EepromIsEmpty_ReturnsFalse()
{
    Controller controller = SetUpController();

    DataPacket *resultPtr = new DataPacket();
    DataPacket result = *resultPtr;
    unsigned int packetSize;
    unsigned int packetAdress;
    bool resultBool = controller.RetrieveDataPacketFromMemory(result, packetSize, packetAdress);

    ASSERT_TEST(resultBool == false);
    delete(resultPtr);
}

void ConvertDataPacketToBareKeyboardKeys_SuccessfullyConvertsPacketIntoListOfBareKeyboardKeys()
{
    Controller controller = SetUpController();
    // Set up packet
    BareKeyboardKey key1, key2;
    key1.pin = 1;
    key1.keyCode = 2;
    key2.pin = 3;
    key2.keyCode = 4;
    BareKeyboardKey data[2] ={
        key1,
        key2,
    };
    uint8_t *dataPtr = (uint8_t*)&data;
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);

    BareKeyboardKey result[2];
    controller.ConvertDataPacketToBareKeyboardKeys(packet, result);

    ASSERT_TEST(result[0].pin == key1.pin && result[0].keyCode == key1.keyCode &&
        result[1].pin == key2.pin && result[1].keyCode == key2.keyCode);
}

void LoadBareKeyboardKeysIntoKeymapList_PopulatestTheListWithTheGivenKeys()
{
    Controller controller = SetUpController();
    unsigned int amountOfKeys = 8;
    BareKeyboardKey key1, key2, key3, key4, key5, key6, key7, key8;
    key1.pin = 1;
    key1.keyCode = 2;
    key2.pin = 3;
    key2.keyCode = 4;
    key3.pin = 5;
    key3.keyCode = 6;
    key4.pin = 7;
    key4.keyCode = 8;
    key5.pin = 9;
    key5.keyCode = 10;
    key6.pin = 11;
    key6.keyCode = 12;
    key7.pin = 13;
    key7.keyCode = 14;
    key8.pin = 15;
    key8.keyCode = 16;
    BareKeyboardKey keys[amountOfKeys] ={ key1, key2, key3, key4, key5, key6, key7, key8 };

    LinkedList<Key*> result = LinkedList<Key*>();
    controller.LoadBareKeyboardKeysIntoKeymapList(keys, amountOfKeys, result);

    bool isEmpty = result[0] == nullptr || result[1] == nullptr;
    Key *resultKeymap1, *resultKeymap2;
    if(!isEmpty) {
        resultKeymap1 = *(result[0]);
        resultKeymap2 = *(result[1]);
    }
    
    ASSERT_TEST(isEmpty == false &&
        resultKeymap1[0].pin == 1 && resultKeymap1[0].keyCode == 2 &&
        resultKeymap1[1].pin == 3 && resultKeymap1[1].keyCode == 4 &&
        resultKeymap1[2].pin == 5 && resultKeymap1[2].keyCode == 6 &&
        resultKeymap1[3].pin == 7 && resultKeymap1[3].keyCode == 8 &&

        resultKeymap2[0].pin == 9 && resultKeymap2[0].keyCode == 10 &&
        resultKeymap2[1].pin == 11 && resultKeymap2[1].keyCode == 12 &&
        resultKeymap2[2].pin == 13 && resultKeymap2[2].keyCode == 14 &&
        resultKeymap2[3].pin == 15 && resultKeymap2[3].keyCode == 16
    );
}

void IsKeyValid_ThePinOfTheKeyIsPresentInTheDefaultKeymap_ReturnsTrue()
{
    const int normalKeyCount = 4;
    Key defaultKeyMap[normalKeyCount] ={
        // Key map Arrow keys
        Key(2, 0),
        Key(3, 0),
        Key(4, 0),
        Key(5, 0),
    };
    SpecialKey specialKeys[0];
    Controller controller(defaultKeyMap, normalKeyCount, specialKeys, 0);
    BareKeyboardKey key;
    key.pin = 2;
    key.keyCode = 1337;

    bool result = controller.IsKeyValid(key);

    ASSERT_TEST(result == true);
}

void IsKeyValid_ThePinOfTheKeyIsNotPresentInTheDefaultKeymap_ReturnsFalse()
{
    const int normalKeyCount = 4;
    Key defaultKeyMap[normalKeyCount] ={
        // Key map Arrow keys
        Key(2, 0),
        Key(3, 0),
        Key(4, 0),
        Key(5, 0),
    };
    SpecialKey specialKeys[0];
    Controller controller(defaultKeyMap, normalKeyCount, specialKeys, 0);
    BareKeyboardKey key;
    key.pin = 94234;
    key.keyCode = 1337;

    bool result = controller.IsKeyValid(key);

    ASSERT_TEST(result == false);
}

// void LoadKeyMapsFromMemory_CorrectlyLoadsKeymapIntoList() {
//     // TODO:  
//     // * Refactor Controller to only use Class variables.
//     // - Setup info to be retrieved through Parse packet.
//     // - Create test for returning false if the pin numbers retrieved are not present in the default keymap.
//     Controller controller = SetUpController();
//     BareKeyboardKey key1, key2, key3, key4;
//     key1.pin = 2;
//     key1.keyCode = 4;
//     key2.pin = 3;
//     key2.keyCode = 26;
//     key3.pin = 4;
//     key3.keyCode = 22;
//     key4.pin = 5;
//     key4.keyCode = 7;
//     BareKeyboardKey data[controller.normalKeyCount] = {
//          key1,
//          key2,
//          key3,
//          key4,
//     };
//     uint8_t *dataPtr = (uint8_t*) &data;
//     DataPacket packet;
//     packet.payloadLength = sizeof(data);
//     packet.payload = dataPtr;
//     packet.crc = CalculateCRC(packet.payload, packet.payloadLength);
//     Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

//     LinkedList<Key *> resultingKeymaps;
//     controller.LoadKeyMapsFromMemory(resultingKeymaps);
//     bool isEmpty = resultingKeymaps[0] == nullptr;
//     printf("%d", isEmpty);
//     printf("\n");
//     Key *result;
//     if(!isEmpty) result = *(resultingKeymaps[0]);

//     ASSERT_TEST(isEmpty == false &&
//                 result[0].pin == data[0].pin &&
//                 result[1].pin == data[1].pin &&
//                 result[2].pin == data[2].pin &&
//                 result[3].pin == data[3].pin);
// }