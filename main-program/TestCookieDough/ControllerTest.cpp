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
extern uint16_t EEPROMClass_length_return;

extern std::vector<int> EEPROMClass_put_param_idx_o1_v;
extern std::vector<uint8_t> EEPROMClass_put_param_t_o1_v;

extern std::vector<uint16_t> EEPROMClass_get_param_t_o1_vr;

extern uint8_t *Serial__write_param_buffer;
extern size_t Serial__write_param_size;

extern std::vector<uint8_t> EEPROMClass_update_param_val_v;

extern std::vector<uint8_t> pinMode_param_pin_v;

const int genericNormalKeyCount = 4;
const int genericSpecialKeyCount = 3;
Controller SetUpController()
{
    BareKeyboardKey defaultKeymap[genericNormalKeyCount] {
        // Key map Arrow keys
        BareKeyboardKey(2, 80),
        BareKeyboardKey(3, 82),
        BareKeyboardKey(4, 81),
        BareKeyboardKey(5, 79),
    };

    SpecialKey specialKeys[genericSpecialKeyCount] {
        SpecialKey(10, toggleEditMode),
        SpecialKey(11, cycleKeyMap),
        SpecialKey(12, toggleDefaultKeyMap),
    };

    return Controller(defaultKeymap, genericNormalKeyCount, specialKeys, genericSpecialKeyCount);
}

void Controller_Constructor_CopiesProvidedKeymapsIntoClass()
{
    // Arrange
    const int expectedNormalKeyCount = 2;
    BareKeyboardKey expectedDefaultKeymap[expectedNormalKeyCount] {
        BareKeyboardKey(2, 10),
        BareKeyboardKey(3, 23),
    };
    const int expectedSpecialKeyCount = 1;
    SpecialKey expectedSpecialKeys[expectedSpecialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    const int tempNormalKeyCount = expectedNormalKeyCount;
    BareKeyboardKey tempDefaultKeymap[tempNormalKeyCount] {
        expectedDefaultKeymap[0],
        expectedDefaultKeymap[1],
    };
    const int tempSpecialKeyCount = expectedSpecialKeyCount;
    SpecialKey tempSpecialKeys[tempSpecialKeyCount] {
        expectedSpecialKeys[0],
    };

    // Act
    Controller controller = Controller(tempDefaultKeymap, tempNormalKeyCount, tempSpecialKeys, tempSpecialKeyCount);
    tempDefaultKeymap[0] = BareKeyboardKey(0,0);
    tempDefaultKeymap[1] = BareKeyboardKey(0,0);
    tempSpecialKeys[0] = SpecialKey(0, cycleKeyMap);
    // Temp values are changed.

    // Assert that the controller was not changed by the temp values.
    ASSERT_TEST(
        !(expectedDefaultKeymap[0] == tempDefaultKeymap[0]) &&
        controller.defaultKeymap[0] == expectedDefaultKeymap[0] &&
        controller.defaultKeymap[1] == expectedDefaultKeymap[1] &&
        controller.normalKeyCount == expectedNormalKeyCount &&
        controller.specialKeys[0] == expectedSpecialKeys[0] &&
        controller.specialKeyCount == expectedSpecialKeyCount
    );
}

void Controller_Constructor_AllocatesEnoughSpaceForPointers() 
{
    Controller controller = SetUpController();
    BareKeyboardKey temp;

    bool didNotCrash = false;
    controller.defaultKeymap[controller.normalKeyCount - 1] = BareKeyboardKey(3, 4);
    controller.specialKeys[controller.specialKeyCount - 1] = SpecialKey(5, toggleDefaultKeyMap);
    controller.currentKeyMap[controller.normalKeyCount - 1] = Key(1, 2);
    controller.buf[controller.bufferSize - 1] = 10;
    controller.customKeyMapsPtr->Add(&temp);
    didNotCrash = true;

    ASSERT_TEST(didNotCrash == true);
}

void RetrieveBareKeyboardKeysFromMemory_RetrievesFaultyData_DoesNotCrashAndReturnsFalse()
{
    Controller controller = SetUpController();
    // Set up packet
    BareKeyboardKey templateData[2];
    const int templateDataSize = sizeof(templateData);

    DataPacket packet;
    uint8_t *dataPtr = new uint8_t[templateDataSize]{0};
    packet.payloadLength = templateDataSize;
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet, eepromSize);

    BareKeyboardKey *result = new BareKeyboardKey[controller.normalKeyCount];
    uint16_t amountOfKeys, packetAdress, packetSize;
    bool doesNotCrash = false;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(&result, &amountOfKeys, &packetAdress, &packetSize);
    doesNotCrash = true;

    ASSERT_TEST(resultBool == false && doesNotCrash == true);
    delete[](result);
    delete[](dataPtr);
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
    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet, eepromSize);
    unsigned int expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(packet);

    BareKeyboardKey *result = new BareKeyboardKey[controller.normalKeyCount];
    uint16_t amountOfKeys, packetAdress, packetSize;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(&result, &amountOfKeys, &packetAdress, &packetSize);

    ASSERT_TEST(resultBool == true &&
                packetSize == expectedPacketSize &&
                packetAdress == 0 &&
                amountOfKeys == 2 &&
                result[0].pin == key1.pin && result[0].keyCode == key1.keyCode &&
                result[1].pin == key2.pin && result[1].keyCode == key2.keyCode);
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
    uint8_t *defectDataPtr = reinterpret_cast<uint8_t *>(&defectData);
    DataPacket defectPacket = DataPacket(defectDataPtr, sizeof(defectData));
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, defectPacket, eepromSize);

    BareKeyboardKey *result = new BareKeyboardKey[controller.normalKeyCount];
    uint16_t amountOfKeys, packetAdress, packetSize;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(&result, &amountOfKeys, &packetAdress, &packetSize);

    ASSERT_TEST(resultBool == false);
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
    uint8_t *defectDataPtr = reinterpret_cast<uint8_t *>(&defectData);
    DataPacket defectPacket = DataPacket(defectDataPtr, sizeof(defectData));
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
    uint8_t *validDataPtr = reinterpret_cast<uint8_t *>(&validData);
    DataPacket validPacket = DataPacket(validDataPtr, sizeof(validData));
    // Prepare packets to be returned.
    unsigned int expectedDefectPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(defectPacket);
    unsigned int expectedValidPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(validPacket);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, defectPacket, eepromSize);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(static_cast<uint16_t>(expectedDefectPacketSize), validPacket, eepromSize);

    BareKeyboardKey *result = new BareKeyboardKey[controller.normalKeyCount];
    uint16_t amountOfKeys, packetAdress, packetSize;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(&result, &amountOfKeys, &packetAdress, &packetSize);

    ASSERT_TEST(resultBool == true &&
                packetAdress == expectedDefectPacketSize &&
                packetSize == expectedValidPacketSize &&
                amountOfKeys == 2 &&
                result[0].pin == validKey1.pin && result[0].keyCode == validKey1.keyCode &&
                result[1].pin == validKey2.pin && result[1].keyCode == validKey2.keyCode);
    delete[](result);
}

void ConvertDataPacketToBareKeyboardKeys_RetrievesCorrectPacketWithFaultyPayload_DoesNotCrash()
{
    Controller controller = SetUpController();
    // Set up packet
    BareKeyboardKey templateData[2];
    const int templateDataSize = sizeof(templateData);

    DataPacket packet;
    uint8_t *dataPtr = new uint8_t[templateDataSize]{0};
    packet.payloadLength = templateDataSize;
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);

    BareKeyboardKey result[2];
    bool didNotCrash = false;
    controller.ConvertDataPacketToBareKeyboardKeys(packet, result);
    uint8_t pinAsInt = result[0].pin;
    pinAsInt++;
    didNotCrash = true;

    ASSERT_TEST(didNotCrash == true);
    delete[](dataPtr);
}

void ConvertDataPacketToBareKeyboardKeys_SuccessfullyConvertsPacketIntoListOfBareKeyboardKeys()
{
    Controller controller = SetUpController();
    // Set up packet
    BareKeyboardKey key1 = BareKeyboardKey(1, 2), 
                    key2 = BareKeyboardKey(3, 4);
    BareKeyboardKey data[2] = {
        key1,
        key2,
    };
    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    BareKeyboardKey result[2];
    controller.ConvertDataPacketToBareKeyboardKeys(packet, result);

    ASSERT_TEST(result[0] == key1 && result[1] == key2);
}

void ParseBareKeyboardKeyArrayIntoKeymapList_PopulatesTheListWithTheGivenKeys()
{
    Controller controller = SetUpController();
    const unsigned int amountOfKeys = 8;
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

    LinkedList<BareKeyboardKey *> result = LinkedList<BareKeyboardKey *>();
    controller.ParseBareKeyboardKeyArrayIntoKeymapList(keys, amountOfKeys, &result);

    bool isEmpty = result[0] == nullptr || result[1] == nullptr;
    BareKeyboardKey *resultKeymap1 = nullptr, *resultKeymap2 = nullptr;
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
}

void IsKeyValid_ThePinOfTheKeyIsPresentInTheDefaultKeymap_ReturnsTrue()
{
    const int normalKeyCount = 4;
    BareKeyboardKey defaultKeymap[normalKeyCount] = {
        // Key map Arrow keys
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 0),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    const int specialKeyCount = 1;
    SpecialKey specialKeys[specialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    Controller controller(defaultKeymap, normalKeyCount, specialKeys, specialKeyCount);
    BareKeyboardKey key;
    key.pin = 2;
    key.keyCode = 1337;

    bool result = controller.IsKeyValid(key.pin);

    ASSERT_TEST(result == true);
}

void IsKeyValid_ThePinOfTheKeyIsNotPresentInTheDefaultKeymap_ReturnsFalse()
{
    const int normalKeyCount = 4;
    BareKeyboardKey defaultKeymapConfiguration[normalKeyCount] = {
        // Key map Arrow keys
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 0),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    const int specialKeyCount = 1;
    SpecialKey specialKeysConfiguration[specialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    Controller controller(defaultKeymapConfiguration, normalKeyCount, specialKeysConfiguration, specialKeyCount);
    BareKeyboardKey key;
    key.pin = 52;
    key.keyCode = 1337;

    bool result = controller.IsKeyValid(key.pin);

    ASSERT_TEST(result == false);
}

void LoadKeymapsFromMemoryIntoList_CorrectlyLoadsKeymapIntoList()
{
    Controller controller = SetUpController();
    BareKeyboardKey key1(2, 4), 
                    key2(3, 26), 
                    key3(4, 22),
                    key4(5, 7);
    BareKeyboardKey data[genericNormalKeyCount] = {
        key1, key2, key3, key4,
    };
    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.LoadKeymapsFromMemoryIntoList(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (!resultingKeymaps.IsEmpty())
        result = *(resultingKeymaps[0]);

    ASSERT_TEST(
        resultingKeymaps.IsEmpty() == false &&
        result[0] == data[0] &&
        result[1] == data[1] &&
        result[2] == data[2] && 
        result[3] == data[3]
    );
}

void LoadKeymapsFromMemoryIntoList_LoadsKeymap_SetsEepromAdressToTheLoadedPacketsAdress()
{
    Controller controller = SetUpController();
    controller.currentPacketAdress = 0;
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 2,
             expectedEepromAdress = packetAdress,
             eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(9);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(packetAdress, packet, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.LoadKeymapsFromMemoryIntoList(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (resultingKeymaps.IsEmpty() == false)
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        controller.currentPacketAdress == expectedEepromAdress &&
        resultingKeymaps.IsEmpty() == false
    );
}

void LoadKeymapsFromMemoryIntoList_LoadsKeymap_SetsNextFreeEepromAdressToAFreeAdressAfterTheEndOfTheLoadedPacket()
{
    Controller controller = SetUpController();
    controller.nextPacketAdress = 0;
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 2,
             expectedNextFreeEepromAdress = static_cast<uint16_t>(packetAdress + Helper_CalculateSizeOfPacketOnEEPROM(packet)),
             eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(9);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(packetAdress, packet, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.LoadKeymapsFromMemoryIntoList(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (resultingKeymaps.IsEmpty() == false)
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        controller.nextPacketAdress == expectedNextFreeEepromAdress &&
        resultingKeymaps.IsEmpty() == false
    );
}

void LoadKeymapsFromMemoryIntoList_EepromHasDefectKeymaps_DoesNotLoadKeymaps()
{
    const int normalKeyCount = 4;
    BareKeyboardKey defaultKeymapConfiguration[normalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 0),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    const int specialKeyCount = 1;
    SpecialKey specialKeysConfiguration[specialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    Controller controller(defaultKeymapConfiguration, normalKeyCount, specialKeysConfiguration, specialKeyCount);
    BareKeyboardKey key1(111, 4), 
                    key2(24, 26), 
                    key3(91, 22), 
                    key4(87, 7);
    BareKeyboardKey data[normalKeyCount] = {
        key1, key2, key3, key4
    };
    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.LoadKeymapsFromMemoryIntoList(&resultingKeymaps);

    ASSERT_TEST(resultingKeymaps.IsEmpty());
}

void LoadKeymapsFromMemoryIntoList_EepromHasDefectKeymapsFollowedByValidKeymaps_LoadsTheValidKeymaps()
{
    const int normalKeyCount = 4;
    BareKeyboardKey defaultKeymapConfiguration[normalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 0),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    const int specialKeyCount = 1;
    SpecialKey specialKeysConfiguration[specialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    Controller controller(defaultKeymapConfiguration, normalKeyCount, specialKeysConfiguration, specialKeyCount);
    BareKeyboardKey defectKey1(140, 4), 
                    defectKey2(75, 26), 
                    defectKey3(86, 22), 
                    defectKey4(188, 7), 
                    validKey1(2, 4), 
                    validKey2(3, 26), 
                    validKey3(4, 22), 
                    validKey4(5, 7);
    BareKeyboardKey defectData[normalKeyCount] = {
        defectKey1, defectKey2, defectKey3, defectKey4,
    };
    BareKeyboardKey validData[normalKeyCount] = {
        validKey1, validKey2, validKey3, validKey4,
    };
    // Set up valid packet.
    uint8_t *validDataPtr = reinterpret_cast<uint8_t *>(&validData);
    DataPacket validPacket = DataPacket(validDataPtr, sizeof(validData));
    // Set up defect packet.
    uint8_t *defectDataPtr = reinterpret_cast<uint8_t *>(&defectData);
    DataPacket defectPacket = DataPacket(defectDataPtr, sizeof(defectData));
    uint16_t validPacketAdress = Helper_CalculateSizeOfPacketOnEEPROM(defectPacket);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, defectPacket, eepromSize);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(validPacketAdress, validPacket, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.LoadKeymapsFromMemoryIntoList(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (!resultingKeymaps.IsEmpty())
        result = *(resultingKeymaps[0]);

    ASSERT_TEST(
        resultingKeymaps.IsEmpty() == false &&
        result[0] == validData[0] &&
        result[1] == validData[1] &&
        result[2] == validData[2] && 
        result[3] == validData[3]
    );
}

void WipeKeyboardEventBuffer_BufferOnlyContainsZeroes()
{
    Controller controller = SetUpController();
    controller.buf[0] = 1;
    controller.buf[3] = 5;
    controller.buf[6] = 7;

    controller.WipeKeyboardEventBuffer();

    ASSERT_TEST(
        controller.buf[0] == 0 &&
        controller.buf[1] == 0 &&
        controller.buf[2] == 0 &&
        controller.buf[3] == 0 &&
        controller.buf[4] == 0 &&
        controller.buf[5] == 0 &&
        controller.buf[6] == 0 &&
        controller.buf[7] == 0);
}

void SendKeyboardEvent_CallsSerialWriteWithTheCorrectBufferSize()
{
    Controller controller = SetUpController();

    controller.SendKeyboardEvent();

    ASSERT_TEST(Serial__write_param_size == 8);
}

void SendKeyboardEvent_CallsSerialWriteWithTheCorrectBuffer()
{
    Controller controller = SetUpController();
    uint8_t expectedBuffer[8];
    controller.buf[0] = expectedBuffer[0] = 1;
    controller.buf[1] = expectedBuffer[1] = 5;
    controller.buf[2] = expectedBuffer[2] = 8;
    controller.buf[3] = expectedBuffer[3] = 9;
    controller.buf[4] = expectedBuffer[4] = 8;
    controller.buf[5] = expectedBuffer[5] = 2;
    controller.buf[6] = expectedBuffer[6] = 8;
    controller.buf[7] = expectedBuffer[7] = 6;

    controller.SendKeyboardEvent();

    ASSERT_TEST(
        Serial__write_param_buffer[0] == expectedBuffer[0] &&
        Serial__write_param_buffer[1] == expectedBuffer[1] &&
        Serial__write_param_buffer[2] == expectedBuffer[2] &&
        Serial__write_param_buffer[3] == expectedBuffer[3] &&
        Serial__write_param_buffer[4] == expectedBuffer[4] &&
        Serial__write_param_buffer[5] == expectedBuffer[5] &&
        Serial__write_param_buffer[6] == expectedBuffer[6] &&
        Serial__write_param_buffer[7] == expectedBuffer[7]);
}

void ChangeKeyMap_TheDefaultKeymapIsEquipped_isUsingDefaultKeymapIsAssignedToTrue()
{
    Controller controller = SetUpController();

    controller.ChangeKeyMap(controller.defaultKeymap);

    ASSERT_TEST(controller.isUsingDefaultKeymap == true);
}

void ChangeKeyMap_ACustomKeymapIsEquipped_isUsingDefaultKeymapIsAssignedFalse()
{
    Controller controller = SetUpController();
    BareKeyboardKey customKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 1),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(4, 3),
        BareKeyboardKey(5, 4),
    };

    controller.ChangeKeyMap(customKeymap);

    ASSERT_TEST(controller.isUsingDefaultKeymap == false);
}

void ChangeKeyMap_ACustomKeymapWithSimilarButNotTheSameSettingsAsDefaultKeymapIsEquipped_isUsingDefaultKeymapIsAssignedFalse()
{
    const int normalKeyCount = 4;
    BareKeyboardKey defaultKeymapConfiguration[normalKeyCount] = {
        BareKeyboardKey(2, 1),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    const int specialKeyCount = 1;
    SpecialKey specialKeysConfiguration[specialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    Controller controller(defaultKeymapConfiguration, normalKeyCount, specialKeysConfiguration, specialKeyCount);
    BareKeyboardKey customKeymap[normalKeyCount] = {
        defaultKeymapConfiguration[0],
        defaultKeymapConfiguration[1],
        defaultKeymapConfiguration[2],
        BareKeyboardKey(5, 44),
    };

    controller.ChangeKeyMap(customKeymap);

    ASSERT_TEST(controller.isUsingDefaultKeymap == false);
}

void ChangeKeyMap_DefaultKeymapIsSelectedAndWeAreEquippingACustomKeymap_CurrentKeymapNowContainsKeysOfCustomKeymap()
{
    Controller controller = SetUpController();
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 12),
        BareKeyboardKey(3, 23),
        BareKeyboardKey(4, 34),
        BareKeyboardKey(5, 45),
    };

    controller.ChangeKeyMap(controller.defaultKeymap);
    controller.ChangeKeyMap(expectedKeymap);

    ASSERT_TEST(
        controller.currentKeyMap[0].pin == expectedKeymap[0].pin && controller.currentKeyMap[0].keyCode == expectedKeymap[0].keyCode &&
        controller.currentKeyMap[1].pin == expectedKeymap[1].pin && controller.currentKeyMap[1].keyCode == expectedKeymap[1].keyCode &&
        controller.currentKeyMap[2].pin == expectedKeymap[2].pin && controller.currentKeyMap[2].keyCode == expectedKeymap[2].keyCode &&
        controller.currentKeyMap[3].pin == expectedKeymap[3].pin && controller.currentKeyMap[3].keyCode == expectedKeymap[3].keyCode);
}

void ChangeKeyMap_EmptiesBufferAndSendsItAsAKeyReleaseEventForAllKeys()
{
    // Arrange
    Controller controller = SetUpController();
    BareKeyboardKey keymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 12),
        BareKeyboardKey(3, 23),
        BareKeyboardKey(4, 34),
        BareKeyboardKey(5, 45),
    };
    size_t expectedBufSize = 8;
    controller.buf[1] = 1;
    controller.buf[3] = 4;
    controller.buf[5] = 9;

    // Act
    controller.ChangeKeyMap(keymap);

    // Assert
    bool serialHasBeenCalled = Serial__write_param_buffer != nullptr;
    bool bufSentIsEmpty = true;
    bool controllerBufIsEmpty = true;
    for (unsigned int i = 0; i < expectedBufSize && serialHasBeenCalled; i++)
    {
        if (!Serial__write_param_buffer[i] == 0)
            bufSentIsEmpty = false;

        if (!controller.buf[i] == 0)
            controllerBufIsEmpty = false;
    }
    ASSERT_TEST(bufSentIsEmpty && controllerBufIsEmpty && serialHasBeenCalled && Serial__write_param_size == expectedBufSize);
}

void ChangeKeyMap_ConfiguresThePinsOfTheProvidedKeymap()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap[genericNormalKeyCount] = {
        BareKeyboardKey(34, 12),
        BareKeyboardKey(6, 23),
        BareKeyboardKey(21, 34),
        BareKeyboardKey(8, 45),
    };

    controller.ChangeKeyMap(keymap);

    ASSERT_TEST(
        pinMode_param_pin_v[0] == keymap[0].pin &&
        pinMode_param_pin_v[1] == keymap[1].pin &&
        pinMode_param_pin_v[2] == keymap[2].pin &&
        pinMode_param_pin_v[3] == keymap[3].pin
    );
}

void CycleKeyMap_TheDefaultKeymapIsCurrentlyEquipped_EquipsTheFirstKeymapInTheList()
{
    Controller controller = SetUpController();
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey keymap2[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4),
        BareKeyboardKey(3, 5),
        BareKeyboardKey(4, 6),
        BareKeyboardKey(5, 7),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(expectedKeymap);
    controller.customKeyMaps.Add(keymap2);
    controller.ChangeKeyMap(controller.defaultKeymap);

    controller.CycleKeyMap();

    ASSERT_TEST(
        expectedKeymap[0].pin == controller.currentKeyMap[0].pin && expectedKeymap[0].keyCode == controller.currentKeyMap[0].keyCode &&
        expectedKeymap[1].pin == controller.currentKeyMap[1].pin && expectedKeymap[1].keyCode == controller.currentKeyMap[1].keyCode &&
        expectedKeymap[2].pin == controller.currentKeyMap[2].pin && expectedKeymap[2].keyCode == controller.currentKeyMap[2].keyCode &&
        expectedKeymap[3].pin == controller.currentKeyMap[3].pin && expectedKeymap[3].keyCode == controller.currentKeyMap[3].keyCode);
}

void CycleKeyMap_TheDefaultKeymapIsCurrentlyEquippedAndWeCycleTwice_EquipsTheSecondKeymapInTheList()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4),
        BareKeyboardKey(3, 5),
        BareKeyboardKey(4, 6),
        BareKeyboardKey(5, 7),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    controller.customKeyMaps.Add(expectedKeymap);
    controller.ChangeKeyMap(controller.defaultKeymap);

    controller.CycleKeyMap();
    controller.CycleKeyMap();

    ASSERT_TEST(
        expectedKeymap[0].pin == controller.currentKeyMap[0].pin && expectedKeymap[0].keyCode == controller.currentKeyMap[0].keyCode &&
        expectedKeymap[1].pin == controller.currentKeyMap[1].pin && expectedKeymap[1].keyCode == controller.currentKeyMap[1].keyCode &&
        expectedKeymap[2].pin == controller.currentKeyMap[2].pin && expectedKeymap[2].keyCode == controller.currentKeyMap[2].keyCode &&
        expectedKeymap[3].pin == controller.currentKeyMap[3].pin && expectedKeymap[3].keyCode == controller.currentKeyMap[3].keyCode);
}

void CycleKeyMap_TheDefaultKeymapIsCurrentlyEquippedAndWeOnlyhaveTwoCustomKeymapsAndWeCycleThreeTimes_WhenWeCycleOffTheLastCustomKeymapItLoopsBackToTheFirstKeymapInTheList()
{
    Controller controller = SetUpController();
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey keymap2[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4),
        BareKeyboardKey(3, 5),
        BareKeyboardKey(4, 6),
        BareKeyboardKey(5, 7),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(expectedKeymap);
    controller.customKeyMaps.Add(keymap2);
    controller.ChangeKeyMap(controller.defaultKeymap);

    controller.CycleKeyMap();
    controller.CycleKeyMap();
    controller.CycleKeyMap();

    ASSERT_TEST(
        expectedKeymap[0].pin == controller.currentKeyMap[0].pin && expectedKeymap[0].keyCode == controller.currentKeyMap[0].keyCode &&
        expectedKeymap[1].pin == controller.currentKeyMap[1].pin && expectedKeymap[1].keyCode == controller.currentKeyMap[1].keyCode &&
        expectedKeymap[2].pin == controller.currentKeyMap[2].pin && expectedKeymap[2].keyCode == controller.currentKeyMap[2].keyCode &&
        expectedKeymap[3].pin == controller.currentKeyMap[3].pin && expectedKeymap[3].keyCode == controller.currentKeyMap[3].keyCode);
}

void SaveKeyMapsToMemory_PutsDownKeysAsBareKeyboardKeyArrayIntoEEPROM()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey keymap2[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4),
        BareKeyboardKey(3, 5),
        BareKeyboardKey(4, 6),
        BareKeyboardKey(5, 7),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount * 2]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3],
        keymap2[0], keymap2[1], keymap2[2], keymap2[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    controller.customKeyMaps.Add(keymap2);
    DataPacket expectedPacket = DataToPacket(expectedData);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(controller.currentPacketAdress, expectedPacket.payload, expectedPacket.payloadLength);

    controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    bool success = true;
    for (uint16_t i = 0; i < expectedPacket.payloadLength; i++)
    {
        if (EEPROMClass_update_param_val_v[i] != expectedPacket.payload[i])
        {
            success = false;
        } 
    }

    ASSERT_TEST(success == true);
}

void SaveKeyMapsToMemory_Succeeds_ReturnsTrue()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey keymap2[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4),
        BareKeyboardKey(3, 5),
        BareKeyboardKey(4, 6),
        BareKeyboardKey(5, 7),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount * 2]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3],
        keymap2[0], keymap2[1], keymap2[2], keymap2[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    controller.customKeyMaps.Add(keymap2);
    DataPacket expectedPacket = DataToPacket(expectedData);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(controller.currentPacketAdress, expectedPacket.payload, expectedPacket.payloadLength);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    bool success = true;
    for (uint16_t i = 0; i < expectedPacket.payloadLength; i++)
    {
        if (EEPROMClass_update_param_val_v[i] != expectedPacket.payload[i])
        {
            success = false;
        } 
    }

    ASSERT_TEST(
        success == true &&
        resultBool == true
    );
}

void SaveKeyMapsToMemory_UpdatesCurrentPacketAdressWithTheAdressOfTheSavedPacket()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    uint16_t expectedCurrentAdress = 124;
    controller.currentPacketAdress = 0;
    controller.nextPacketAdress = 124;
    // Setup mocked packet to return so that the function succeeds.
    DataPacket packet = DataToPacket(expectedData);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(expectedCurrentAdress, packet.payload, packet.payloadLength);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    ASSERT_TEST(
        resultBool == true &&
        controller.currentPacketAdress == expectedCurrentAdress
    );
}

void SaveKeyMapsToMemory_UpdatesNextPacketAdressWithAFreeAdress()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    DataPacket packet = DataToPacket(expectedData);
    unsigned int packetSize = Helper_CalculateSizeOfPacketOnEEPROM(packet);
    uint16_t adressToSavePacket = 13;
    uint16_t expectedNextPacketAdress = static_cast<uint16_t>(adressToSavePacket + packetSize);
    controller.currentPacketAdress = 0;
    controller.nextPacketAdress = adressToSavePacket;
    // Setup mocked packet to return so that the function succeeds.
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adressToSavePacket, packet.payload, packet.payloadLength);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    ASSERT_TEST(
        resultBool == true &&
        controller.nextPacketAdress == expectedNextPacketAdress
    );
}

void SaveKeyMapsToMemory_PacketIsSavedAtTheNextPacketAdress()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    uint16_t expectedAdress = 124;
    controller.currentPacketAdress = 0;
    controller.nextPacketAdress = expectedAdress;
    // Setup mocked packet to return so that the function succeeds.
    DataPacket packet = DataToPacket(expectedData);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(expectedAdress, packet.payload, packet.payloadLength);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    ASSERT_TEST(
        resultBool == true &&
        EEPROMClass_put_param_idx_o1_v[0] == expectedAdress &&
        EEPROMClass_put_param_t_o1_v[0] == packet.stx
    );
}

void SaveKeyMapsToMemory_PacketIsSavedSuccessfully_DeactivatesOldPacket()
{
    // Arrange
    uint16_t deactivatedFlag = 0x00;
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    DataPacket packet = DataToPacket(expectedData);
    DataPacket oldPacket = DataToPacket(expectedData);
    uint16_t expectedOldPacketAdress = 13;
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>(expectedOldPacketAdress + sizeof(packet.stx));
    controller.currentPacketAdress = expectedOldPacketAdress;
    controller.nextPacketAdress = static_cast<uint16_t>(expectedOldPacketAdress + Helper_CalculateSizeOfPacketOnEEPROM(packet) + 10);
    // Setup mocked packet to return so that the function succeeds.
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(controller.nextPacketAdress, packet.payload, packet.payloadLength);
    EEPROMClass_read_return_v.push_back(oldPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(oldPacket.payloadLength);
    EEPROMClass_read_return_v.push_back(oldPacket.etx);

    // Act
    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    // Assert
    bool testHasSucceeded = (EEPROMClass_put_param_idx_o1_v.size() > 3) ? (
            resultBool == true &&
            EEPROMClass_put_param_idx_o1_v.at(3) == static_cast<int>(expectedOverwrittenAdress) &&
            EEPROMClass_put_param_t_o1_v.at(3) == deactivatedFlag
        ) : false;
    ASSERT_TEST(testHasSucceeded);
}

void SaveKeyMapsToMemory_CurrentPacketAdressAndNextPacketAdressAreTheSame_SuccessfullySavesPacket_DoesNotDeactivateTheSavedPacket()
{
    // Arrange
    uint16_t deactivatedFlag = 0x00;
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    DataPacket packet = DataToPacket(expectedData);
    DataPacket oldPacket = DataToPacket(expectedData);
    uint16_t sharedAdress = 13;
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>(sharedAdress + sizeof(packet.stx));
    controller.currentPacketAdress = sharedAdress;
    controller.nextPacketAdress = sharedAdress;
    // Setup mocked packet to return so that the function succeeds.
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(controller.nextPacketAdress, packet.payload, packet.payloadLength);
    EEPROMClass_read_return_v.push_back(oldPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(oldPacket.payloadLength);
    EEPROMClass_read_return_v.push_back(oldPacket.etx);

    // Act
    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    // Assert
    bool testHasSucceeded = (EEPROMClass_put_param_idx_o1_v.size() > 3) ? (
            resultBool == true &&
            EEPROMClass_put_param_idx_o1_v.at(3) == static_cast<int>(expectedOverwrittenAdress) &&
            EEPROMClass_put_param_t_o1_v.at(3) != deactivatedFlag
        ) : resultBool == true;
    ASSERT_TEST(testHasSucceeded);
}

void SaveKeyMapsToMemory_FailedToSavePacket_DoesNotDeactivateOldPacket()
{
    // Arrange
    uint16_t deactivatedFlag = 0x00;
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    DataPacket packet = DataToPacket(expectedData);
    DataPacket oldPacket = DataToPacket(expectedData);
    uint16_t expectedOldPacketAdress = 13;
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>(expectedOldPacketAdress + sizeof(packet.stx));
    controller.currentPacketAdress = expectedOldPacketAdress;
    controller.nextPacketAdress = static_cast<uint16_t>(expectedOldPacketAdress + Helper_CalculateSizeOfPacketOnEEPROM(packet) + 10);
    // Add garbage data
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(oldPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(oldPacket.payloadLength);
    EEPROMClass_read_return_v.push_back(oldPacket.etx);

    // Act
    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    // Assert
    bool testHasSucceeded = (EEPROMClass_put_param_idx_o1_v.size() > 3) ? (
            resultBool == false &&
            EEPROMClass_put_param_idx_o1_v.at(3) == static_cast<int>(expectedOverwrittenAdress) &&
            EEPROMClass_put_param_t_o1_v.at(3) != deactivatedFlag
        ) : resultBool == false;
    ASSERT_TEST(testHasSucceeded);
}

void SaveKeyMapsToMemory_TheFunctionFailsBecauseTheSavedPacketCanNotBeRead_ReturnsFalse()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    BareKeyboardKey keymap2[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4),
        BareKeyboardKey(3, 5),
        BareKeyboardKey(4, 6),
        BareKeyboardKey(5, 7),
    };
    BareKeyboardKey expectedData[genericNormalKeyCount * 2]
    {
        keymap1[0], keymap1[1], keymap1[2], keymap1[3],
        keymap2[0], keymap2[1], keymap2[2], keymap2[3]
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    controller.customKeyMaps.Add(keymap2);
    DataPacket expectedPacket = DataToPacket(expectedData);
    // Add garbage data
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(77);
    EEPROMClass_read_return_v.push_back(23);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    ASSERT_TEST(
        resultBool == false
    );
}

void ToggleEditMode_CurrentKeymapHasBeenEdited_UpdatesTheEquippedCustomKeymapWithTheValuesPresentInCurrentKeymap()
{
    Controller controller = SetUpController();
    BareKeyboardKey customKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 0),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 11),
        BareKeyboardKey(3, 22),
        BareKeyboardKey(4, 33),
        BareKeyboardKey(5, 44),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(customKeymap);
    controller.ChangeKeyMap(controller.defaultKeymap);
    controller.CycleKeyMap(); // Selects the first custom keymap.
    controller.currentKeyMap[0] = Key(expectedKeymap[0].pin, expectedKeymap[0].keyCode);
    controller.currentKeyMap[1] = Key(expectedKeymap[1].pin, expectedKeymap[1].keyCode);
    controller.currentKeyMap[2] = Key(expectedKeymap[2].pin, expectedKeymap[2].keyCode);
    controller.currentKeyMap[3] = Key(expectedKeymap[3].pin, expectedKeymap[3].keyCode);

    controller.ToggleEditMode();
    BareKeyboardKey *resultingCustomKeymap = (*controller.customKeyMaps[0]);

    ASSERT_TEST(
        resultingCustomKeymap[0].pin == expectedKeymap[0].pin && resultingCustomKeymap[0].keyCode == expectedKeymap[0].keyCode &&
        resultingCustomKeymap[1].pin == expectedKeymap[1].pin && resultingCustomKeymap[1].keyCode == expectedKeymap[1].keyCode &&
        resultingCustomKeymap[2].pin == expectedKeymap[2].pin && resultingCustomKeymap[2].keyCode == expectedKeymap[2].keyCode &&
        resultingCustomKeymap[3].pin == expectedKeymap[3].pin && resultingCustomKeymap[3].keyCode == expectedKeymap[3].keyCode);
}

void SaveControllerSettings_CurrentKeymapHasBeenEdited_UpdatesTheEquippedCustomKeymapWithTheValuesPresentInCurrentKeymap()
{
    EEPROMClass_length_return = 1024;
    Controller controller = SetUpController();
    BareKeyboardKey customKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 0),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 11),
        BareKeyboardKey(3, 22),
        BareKeyboardKey(4, 33),
        BareKeyboardKey(5, 44),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(customKeymap);
    controller.ChangeKeyMap(controller.defaultKeymap);
    controller.CycleKeyMap(); // Selects the first custom keymap.
    controller.currentKeyMap[0] = Key(expectedKeymap[0].pin, expectedKeymap[0].keyCode);
    controller.currentKeyMap[1] = Key(expectedKeymap[1].pin, expectedKeymap[1].keyCode);
    controller.currentKeyMap[2] = Key(expectedKeymap[2].pin, expectedKeymap[2].keyCode);
    controller.currentKeyMap[3] = Key(expectedKeymap[3].pin, expectedKeymap[3].keyCode);

    controller.SaveControllerSettings();
    BareKeyboardKey *resultingCustomKeymap = (*controller.customKeyMaps[0]);

    ASSERT_TEST(
        resultingCustomKeymap[0].pin == expectedKeymap[0].pin && resultingCustomKeymap[0].keyCode == expectedKeymap[0].keyCode &&
        resultingCustomKeymap[1].pin == expectedKeymap[1].pin && resultingCustomKeymap[1].keyCode == expectedKeymap[1].keyCode &&
        resultingCustomKeymap[2].pin == expectedKeymap[2].pin && resultingCustomKeymap[2].keyCode == expectedKeymap[2].keyCode &&
        resultingCustomKeymap[3].pin == expectedKeymap[3].pin && resultingCustomKeymap[3].keyCode == expectedKeymap[3].keyCode);
}

void UpdateCurrentCustomKeymap_CurrentKeymapHasBeenEdited_UpdatesTheEquippedCustomKeymapWithTheValuesPresentInCurrentKeymap()
{
    Controller controller = SetUpController();
    BareKeyboardKey customKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 0),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 11),
        BareKeyboardKey(3, 22),
        BareKeyboardKey(4, 33),
        BareKeyboardKey(5, 44),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(customKeymap);
    controller.ChangeKeyMap(controller.defaultKeymap);
    controller.CycleKeyMap(); // Selects the first custom keymap.
    controller.currentKeyMap[0] = Key(expectedKeymap[0].pin, expectedKeymap[0].keyCode);
    controller.currentKeyMap[1] = Key(expectedKeymap[1].pin, expectedKeymap[1].keyCode);
    controller.currentKeyMap[2] = Key(expectedKeymap[2].pin, expectedKeymap[2].keyCode);
    controller.currentKeyMap[3] = Key(expectedKeymap[3].pin, expectedKeymap[3].keyCode);

    controller.UpdateCurrentCustomKeymap();
    BareKeyboardKey *resultingCustomKeymap = (*controller.customKeyMaps[0]);

    ASSERT_TEST(
        resultingCustomKeymap[0].pin == expectedKeymap[0].pin && resultingCustomKeymap[0].keyCode == expectedKeymap[0].keyCode &&
        resultingCustomKeymap[1].pin == expectedKeymap[1].pin && resultingCustomKeymap[1].keyCode == expectedKeymap[1].keyCode &&
        resultingCustomKeymap[2].pin == expectedKeymap[2].pin && resultingCustomKeymap[2].keyCode == expectedKeymap[2].keyCode &&
        resultingCustomKeymap[3].pin == expectedKeymap[3].pin && resultingCustomKeymap[3].keyCode == expectedKeymap[3].keyCode);
}

void UpdateCurrentCustomKeymap_CurrentKeymapHasBeenEditedAndTheSecondCustomKeymapIsSelected_UpdatesTheSelectedCustomKeymap()
{
    Controller controller = SetUpController();
    BareKeyboardKey customKeymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 1),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 1),
        BareKeyboardKey(5, 1),
    };
    BareKeyboardKey customKeymap2[genericNormalKeyCount] = {
        BareKeyboardKey(2, 2),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 2),
    };
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 11),
        BareKeyboardKey(3, 22),
        BareKeyboardKey(4, 33),
        BareKeyboardKey(5, 44),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(customKeymap1);
    controller.customKeyMaps.Add(customKeymap2);
    controller.ChangeKeyMap(controller.defaultKeymap);
    // Selects the second keymap.
    controller.CycleKeyMap();
    controller.CycleKeyMap();
    // Updates the currentKeymap with the new values.
    controller.currentKeyMap[0] = Key(expectedKeymap[0].pin, expectedKeymap[0].keyCode);
    controller.currentKeyMap[1] = Key(expectedKeymap[1].pin, expectedKeymap[1].keyCode);
    controller.currentKeyMap[2] = Key(expectedKeymap[2].pin, expectedKeymap[2].keyCode);
    controller.currentKeyMap[3] = Key(expectedKeymap[3].pin, expectedKeymap[3].keyCode);

    controller.UpdateCurrentCustomKeymap();
    BareKeyboardKey *resultingSecondCustomKeymap = (*controller.customKeyMaps[1]);

    ASSERT_TEST(
        resultingSecondCustomKeymap[0].pin == expectedKeymap[0].pin && resultingSecondCustomKeymap[0].keyCode == expectedKeymap[0].keyCode &&
        resultingSecondCustomKeymap[1].pin == expectedKeymap[1].pin && resultingSecondCustomKeymap[1].keyCode == expectedKeymap[1].keyCode &&
        resultingSecondCustomKeymap[2].pin == expectedKeymap[2].pin && resultingSecondCustomKeymap[2].keyCode == expectedKeymap[2].keyCode &&
        resultingSecondCustomKeymap[3].pin == expectedKeymap[3].pin && resultingSecondCustomKeymap[3].keyCode == expectedKeymap[3].keyCode);
}

void UpdateCurrentCustomKeymap_CurrentKeymapHasBeenEditedAndTheSecondCustomKeymapIsSelected_DoesNotChangeTheOtherKeymaps()
{
    Controller controller = SetUpController();
    BareKeyboardKey customKeymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 123),
        BareKeyboardKey(3, 123),
        BareKeyboardKey(4, 123),
        BareKeyboardKey(5, 123),
    };
    BareKeyboardKey customKeymap2[genericNormalKeyCount] = {
        BareKeyboardKey(2, 2),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 2),
    };
    BareKeyboardKey editedCurrentKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 0),
        BareKeyboardKey(4, 0),
        BareKeyboardKey(5, 0),
    };
    BareKeyboardKey expectedKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 123),
        BareKeyboardKey(3, 123),
        BareKeyboardKey(4, 123),
        BareKeyboardKey(5, 123),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(customKeymap1);
    controller.customKeyMaps.Add(customKeymap2);
    controller.ChangeKeyMap(controller.defaultKeymap);
    // Selects the second keymap.
    controller.CycleKeyMap();
    controller.CycleKeyMap();
    // Updates the currentKeymap with the new values.
    controller.currentKeyMap[0] = Key(editedCurrentKeymap[0].pin, editedCurrentKeymap[0].keyCode);
    controller.currentKeyMap[1] = Key(editedCurrentKeymap[1].pin, editedCurrentKeymap[1].keyCode);
    controller.currentKeyMap[2] = Key(editedCurrentKeymap[2].pin, editedCurrentKeymap[2].keyCode);
    controller.currentKeyMap[3] = Key(editedCurrentKeymap[3].pin, editedCurrentKeymap[3].keyCode);

    controller.UpdateCurrentCustomKeymap();
    BareKeyboardKey *resultingFirstCustomKeymap = (*controller.customKeyMaps[0]);

    ASSERT_TEST(
        resultingFirstCustomKeymap[0].pin == expectedKeymap[0].pin && resultingFirstCustomKeymap[0].keyCode == expectedKeymap[0].keyCode &&
        resultingFirstCustomKeymap[1].pin == expectedKeymap[1].pin && resultingFirstCustomKeymap[1].keyCode == expectedKeymap[1].keyCode &&
        resultingFirstCustomKeymap[2].pin == expectedKeymap[2].pin && resultingFirstCustomKeymap[2].keyCode == expectedKeymap[2].keyCode &&
        resultingFirstCustomKeymap[3].pin == expectedKeymap[3].pin && resultingFirstCustomKeymap[3].keyCode == expectedKeymap[3].keyCode);
}
