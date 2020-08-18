#include "test.h"
#include "../lib/CookieDoughLibrary/Controller.h"
#include "../lib/CookieDoughLibrary/DataPacket.h"
#include "../lib/CookieDoughLibrary/LinkedList.h"
#include "../lib/CookieDoughLibrary/DataPacket.h"
#include "Fakes/Arduino.h"
#include "Fakes/EEPROM.h"
#include "TestHelper/DataPacketHelper.h"

extern std::vector<uint8_t> EEPROMClass_read_return_v;
extern std::vector<int> EEPROMClass_read_param_idx_v;

extern std::vector<uint16_t> EEPROMClass_length_return_v;

Key *defaultKeymap;
SpecialKey *specialKeys;
Controller SetUpController()
{
    const int normalKeyCount = 4;
    defaultKeymap = new Key[normalKeyCount]{
        // Key map Arrow keys
        Key(2, 80),
        Key(3, 82),
        Key(4, 81),
        Key(5, 79),
    };

    const int specialKeyCount = 3;
    specialKeys = new SpecialKey[specialKeyCount]{
        SpecialKey(10, toggleEditMode),
        SpecialKey(11, cycleKeyMap),
        SpecialKey(12, toggleDefaultKeyMap),
    };

    return Controller(defaultKeymap, normalKeyCount, specialKeys, specialKeyCount);
}

void DestroyController()
{
    delete[](defaultKeymap);
    delete[](specialKeys);
}

void RetrieveBareKeyboardKeysFromMemory_FindsPacketAndReturnsTheBareKeyboardKeysInside()
{
    Controller controller = SetUpController();
    // Set up packet
    BareKeyboardKey key1, key2;
    key1.pin = 2;
    key2.pin = 3;
    key1.keyCode = 2;
    key2.keyCode = 4;
    BareKeyboardKey data[2] = {
        key1,
        key2,
    };
    uint8_t *dataPtr = (uint8_t *)&data;
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

    BareKeyboardKey *result = new BareKeyboardKey[controller.normalKeyCount];
    unsigned int amountOfKeys, packetAdress, packetSize;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(result, amountOfKeys, packetAdress, packetSize);

    ASSERT_TEST(resultBool == true &&
                packetSize == 32 &&
                packetAdress == 0 &&
                amountOfKeys == 2 &&
                result[0].pin == key1.pin && result[0].keyCode == key1.keyCode &&
                result[1].pin == key2.pin && result[1].keyCode == key2.keyCode);
    DestroyController();
    delete[](result);
}

void RetrieveBareKeyboardKeysFromMemory_FindsDefectPacket_ReturnsFalse()
{
    Controller controller = SetUpController();
    // Set up packet
    // Set up defect Packet
    BareKeyboardKey defectKey1, defectKey2;
    defectKey1.pin = 123;
    defectKey2.pin = 111;
    defectKey1.keyCode = 2;
    defectKey2.keyCode = 4;
    BareKeyboardKey defectData[2] = {
        defectKey1,
        defectKey2,
    };
    uint8_t *defectDataPtr = (uint8_t *)&defectData;
    DataPacket defectPacket;
    defectPacket.payloadLength = sizeof(defectData);
    defectPacket.payload = defectDataPtr;
    defectPacket.crc = CalculateCRC(defectPacket.payload, defectPacket.payloadLength);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(defectPacket);

    BareKeyboardKey *result = new BareKeyboardKey[controller.normalKeyCount];
    unsigned int amountOfKeys, packetAdress, packetSize;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(result, amountOfKeys, packetAdress, packetSize);

    ASSERT_TEST(resultBool == false);
    DestroyController();
    delete[](result);
}

void RetrieveBareKeyboardKeysFromMemory_EepromHasDefectPacketFollowedByValidPacket_ReturnsKeysFromValidPacket()
{
    Controller controller = SetUpController();
    // Set up defect Packet
    BareKeyboardKey defectKey1, defectKey2;
    defectKey1.pin = 169;
    defectKey2.pin = 222;
    defectKey1.keyCode = 2;
    defectKey2.keyCode = 4;
    BareKeyboardKey defectData[2] = {
        defectKey1,
        defectKey2,
    };
    uint8_t *defectDataPtr = (uint8_t *)&defectData;
    DataPacket defectPacket;
    defectPacket.payloadLength = sizeof(defectData);
    defectPacket.payload = defectDataPtr;
    defectPacket.crc = CalculateCRC(defectPacket.payload, defectPacket.payloadLength);
    // Set up packet
    BareKeyboardKey validKey1, validKey2;
    validKey1.pin = 2;
    validKey2.pin = 3;
    validKey1.keyCode = 2;
    validKey2.keyCode = 4;
    BareKeyboardKey validData[2] = {
        validKey1,
        validKey2,
    };
    uint8_t *validDataPtr = (uint8_t *)&validData;
    DataPacket validPacket;
    validPacket.payloadLength = sizeof(validData);
    validPacket.payload = validDataPtr;
    validPacket.crc = CalculateCRC(validPacket.payload, validPacket.payloadLength);
    // Prepare packets to be returned.
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(defectPacket);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(validPacket);

    BareKeyboardKey *result = new BareKeyboardKey[controller.normalKeyCount];
    unsigned int amountOfKeys, packetAdress, packetSize;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(result, amountOfKeys, packetAdress, packetSize);

    ASSERT_TEST(resultBool == true &&
                packetSize == 32 &&
                packetAdress == 32 &&
                amountOfKeys == 2 &&
                result[0].pin == validKey1.pin && result[0].keyCode == validKey1.keyCode &&
                result[1].pin == validKey2.pin && result[1].keyCode == validKey2.keyCode);
    DestroyController();
    delete[](result);
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
    uint8_t *dataPtr = (uint8_t *)&data;
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
        *((uint16_t *)packet.payload) == data &&
        packet.etx == result.etx &&
        packetAdress == 2 &&
        packetSize == 10);

    delete (resultPtr);
    DestroyController();
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
    delete (resultPtr);
    DestroyController();
}

void RetrieveDataPacketFromMemory_StartAdressIsGiven_BeginsLookingForPacketAtStartAdress()
{
    unsigned int expectedStartAdress = 25;
    Controller controller = SetUpController();
    DataPacket *resultPtr = new DataPacket();
    DataPacket result = *resultPtr;
    unsigned int packetSize;
    unsigned int packetAdress;
    controller.RetrieveDataPacketFromMemory(result, packetSize, packetAdress, expectedStartAdress);

    ASSERT_TEST(EEPROMClass_read_param_idx_v[0] == expectedStartAdress);
    delete (resultPtr);
    DestroyController();
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
    BareKeyboardKey data[2] = {
        key1,
        key2,
    };
    uint8_t *dataPtr = (uint8_t *)&data;
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);

    BareKeyboardKey result[2];
    controller.ConvertDataPacketToBareKeyboardKeys(packet, result);

    ASSERT_TEST(result[0].pin == key1.pin && result[0].keyCode == key1.keyCode &&
                result[1].pin == key2.pin && result[1].keyCode == key2.keyCode);
    DestroyController();
}

void ParseBareKeyboardKeysIntoKeymapList_PopulatesTheListWithTheGivenKeys()
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
    BareKeyboardKey keys[amountOfKeys] = {key1, key2, key3, key4, key5, key6, key7, key8};

    LinkedList<Key *> result = LinkedList<Key *>();
    controller.ParseBareKeyboardKeysIntoKeymapList(keys, amountOfKeys, result);

    bool isEmpty = result[0] == nullptr || result[1] == nullptr;
    Key *resultKeymap1, *resultKeymap2;
    if (!isEmpty)
    {
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
                resultKeymap2[3].pin == 15 && resultKeymap2[3].keyCode == 16);
    DestroyController();
}

void IsKeyValid_ThePinOfTheKeyIsPresentInTheDefaultKeymap_ReturnsTrue()
{
    const int normalKeyCount = 4;
    Key defaultKeymap[normalKeyCount] = {
        // Key map Arrow keys
        Key(2, 0),
        Key(3, 0),
        Key(4, 0),
        Key(5, 0),
    };
    SpecialKey specialKeys[0];
    Controller controller(defaultKeymap, normalKeyCount, specialKeys, 0);
    BareKeyboardKey key;
    key.pin = 2;
    key.keyCode = 1337;

    bool result = controller.IsKeyValid(key);

    ASSERT_TEST(result == true);
}

void IsKeyValid_ThePinOfTheKeyIsNotPresentInTheDefaultKeymap_ReturnsFalse()
{
    const int normalKeyCount = 4;
    Key defaultKeymapConfiguration[normalKeyCount] = {
        // Key map Arrow keys
        Key(2, 0),
        Key(3, 0),
        Key(4, 0),
        Key(5, 0),
    };
    SpecialKey specialKeysConfiguration[0];
    Controller controller(defaultKeymapConfiguration, normalKeyCount, specialKeysConfiguration, 0);
    BareKeyboardKey key;
    key.pin = 52;
    key.keyCode = 1337;

    bool result = controller.IsKeyValid(key);

    ASSERT_TEST(result == false);
}

void LoadKeymapsFromMemoryIntoList_CorrectlyLoadsKeymapIntoList()
{
    Controller controller = SetUpController();
    BareKeyboardKey key1, key2, key3, key4;
    key1.pin = 2;
    key1.keyCode = 4;
    key2.pin = 3;
    key2.keyCode = 26;
    key3.pin = 4;
    key3.keyCode = 22;
    key4.pin = 5;
    key4.keyCode = 7;
    BareKeyboardKey data[controller.normalKeyCount] = {
        key1,
        key2,
        key3,
        key4,
    };
    uint8_t *dataPtr = (uint8_t *)&data;
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

    LinkedList<Key *> resultingKeymaps = LinkedList<Key *>();
    controller.LoadKeymapsFromMemoryIntoList(resultingKeymaps);
    Key *result;
    if (!resultingKeymaps.IsEmpty())
        result = *(resultingKeymaps[0]);

    ASSERT_TEST(resultingKeymaps.IsEmpty() == false &&
                result[0].pin == data[0].pin &&
                result[1].pin == data[1].pin &&
                result[2].pin == data[2].pin &&
                result[3].pin == data[3].pin);
    DestroyController();
}

void LoadKeymapsFromMemoryIntoList_EepromHasDefectKeymaps_DoesNotLoadKeymaps()
{
    const int normalKeyCount = 4;
    Key defaultKeymapConfiguration[normalKeyCount] = {
        Key(2, 0),
        Key(3, 0),
        Key(4, 0),
        Key(5, 0),
    };
    SpecialKey specialKeysConfiguration[0];
    Controller controller(defaultKeymapConfiguration, normalKeyCount, specialKeysConfiguration, 0);
    BareKeyboardKey key1, key2, key3, key4;
    key1.pin = 111;
    key2.pin = 24;
    key3.pin = 91;
    key4.pin = 87;
    key1.keyCode = 4;
    key2.keyCode = 26;
    key3.keyCode = 22;
    key4.keyCode = 7;
    BareKeyboardKey data[controller.normalKeyCount] = {
        key1,
        key2,
        key3,
        key4,
    };
    uint8_t *dataPtr = (uint8_t *)&data;
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

    LinkedList<Key *> resultingKeymaps = LinkedList<Key *>();
    controller.LoadKeymapsFromMemoryIntoList(resultingKeymaps);

    ASSERT_TEST(resultingKeymaps.IsEmpty());
}

void LoadKeymapsFromMemoryIntoList_EepromHasDefectKeymapsFollowedByValidKeymaps_LoadsTheValidKeymaps() {
    const int normalKeyCount = 4;
    Key defaultKeymapConfiguration[normalKeyCount] = {
        Key(2, 0),
        Key(3, 0),
        Key(4, 0),
        Key(5, 0),
    };
    SpecialKey specialKeysConfiguration[0];
    Controller controller(defaultKeymapConfiguration, normalKeyCount, specialKeysConfiguration, 0);
    BareKeyboardKey defectKey1, defectKey2, defectKey3, defectKey4, validKey1, validKey2, validKey3, validKey4;
    defectKey1.pin = 140;
    defectKey2.pin = 75;
    defectKey3.pin = 86;
    defectKey4.pin = 188;
    defectKey1.keyCode = 4;
    defectKey2.keyCode = 26;
    defectKey3.keyCode = 22;
    defectKey4.keyCode = 7;
    validKey1.pin = 2;
    validKey2.pin = 3;
    validKey3.pin = 4;
    validKey4.pin = 5;
    validKey1.keyCode = 4;
    validKey2.keyCode = 26;
    validKey3.keyCode = 22;
    validKey4.keyCode = 7;
    BareKeyboardKey defectData[controller.normalKeyCount] = {
         defectKey1,
         defectKey2,
         defectKey3,
         defectKey4,
    };
    BareKeyboardKey validData[controller.normalKeyCount] = {
         validKey1,
         validKey2,
         validKey3,
         validKey4,
    };
    // Set up valid packet.
    uint8_t *validDataPtr = (uint8_t*) &validData;
    DataPacket validPacket;
    validPacket.payloadLength = sizeof(validData);
    validPacket.payload = validDataPtr;
    validPacket.crc = CalculateCRC(validPacket.payload, validPacket.payloadLength);
    // Set up defect packet.
    uint8_t *defectDataPtr = (uint8_t*) &defectData;
    DataPacket defectPacket;
    defectPacket.payloadLength = sizeof(defectData);
    defectPacket.payload = defectDataPtr;
    defectPacket.crc = CalculateCRC(defectPacket.payload, defectPacket.payloadLength);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(defectPacket);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(validPacket);

    LinkedList<Key *> resultingKeymaps = LinkedList<Key*>();
    controller.LoadKeymapsFromMemoryIntoList(resultingKeymaps);
    Key *result;
    if(!resultingKeymaps.IsEmpty()) result = *(resultingKeymaps[0]);

    ASSERT_TEST(resultingKeymaps.IsEmpty() == false &&
                result[0].pin == validData[0].pin &&
                result[1].pin == validData[1].pin &&
                result[2].pin == validData[2].pin &&
                result[3].pin == validData[3].pin);
}