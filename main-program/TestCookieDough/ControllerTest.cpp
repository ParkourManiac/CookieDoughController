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

extern unsigned int EEPROMClass_get_invocations_o3;

extern uint8_t *Serial__write_param_buffer;
extern size_t Serial__write_param_size;

extern unsigned int EEPROMClass_update_invocations;
extern std::vector<uint8_t> EEPROMClass_update_param_val_v;


extern std::vector<uint8_t> pinMode_param_pin_v;

extern int freeMemory_return;
extern std::vector<int> freeMemory_return_v;
extern unsigned int freeMemory_invocations;

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

    EEPROMClass_length_return_v.push_back(1024);
    freeMemory_return = 2048;

    return Controller(defaultKeymap, genericNormalKeyCount, specialKeys, genericSpecialKeyCount);
}

void Controller_Constructor_ReadsSizeOfEEPROMAndSavesItToStorageSizeVariable()
{
    uint16_t expectedStorageSize = 1024;
    EEPROMClass_length_return = expectedStorageSize;
    BareKeyboardKey defaultKeymap[1] {
        BareKeyboardKey(2, 80),
    };
    SpecialKey specialKeys[1] {
        SpecialKey(12, toggleDefaultKeyMap),
    };

    Controller controller = Controller(defaultKeymap, 1, specialKeys, 1);
    
    ASSERT_TEST(controller.storageSize == expectedStorageSize);
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

void Controller_Constructor_AmountOfFreeStorageIsEqualToTheStorageSize()
{
    const int amountOfDefaultKeys = 2;
    BareKeyboardKey defaultKeymap[amountOfDefaultKeys] {
        BareKeyboardKey(9, 1),
        BareKeyboardKey(3, 2),
    };
    const int amountOfSpecialKeys = 1;
    SpecialKey specialKeymap[amountOfSpecialKeys] {
        SpecialKey(10, toggleDefaultKeyMap),
    };
    uint16_t eepromSize = 1337;
    EEPROMClass_length_return = eepromSize;
    Controller controller = Controller(defaultKeymap, amountOfDefaultKeys, specialKeymap, amountOfSpecialKeys);

    ASSERT_TEST(
        controller.amountOfFreeStorage == controller.storageSize &&
        controller.amountOfFreeStorage == eepromSize
    );
}

void CyclicEepromAdress_TakesInAnAdressThatExceedsTheEepromsSize_WrapsBackToTheBeginningOfTheEeprom()
{
    Controller controller = SetUpController();
    uint16_t adress = static_cast<uint16_t>(controller.storageSize + 13);
    uint16_t expectedAdress = static_cast<uint16_t>((controller.storageSize + 13) % controller.storageSize);

    uint16_t result = controller.CyclicEepromAdress(adress);

    ASSERT_TEST(result == expectedAdress);
}

void CyclicEepromAdress_AdressOvershootsEepromSizeWithOneStep_ReturnsTheFirstAdressOftheEeprom()
{
    Controller controller = SetUpController();
    uint16_t lastAdressOfStorage = static_cast<uint16_t>(controller.storageSize - 1);
    uint16_t adress = static_cast<uint16_t>(lastAdressOfStorage + 1);
    uint16_t expectedAdress = 0;

    uint16_t result = controller.CyclicEepromAdress(adress);

    ASSERT_TEST(result == expectedAdress);
}

void CyclicEepromAdress_TakesInAnAdressThatExceedsTheEepromsSize_CalculatesTheNewAdressCorrectly()
{
    Controller controller = SetUpController();
    uint16_t lastAdressOfStorage = static_cast<uint16_t>(controller.storageSize - 1);
    uint16_t overshoot = 25;
    uint16_t adress = static_cast<uint16_t>(lastAdressOfStorage + overshoot);
    uint16_t expectedAdress = static_cast<uint16_t>(overshoot - 1);

    uint16_t result = controller.CyclicEepromAdress(adress);

    ASSERT_TEST(result == expectedAdress);
}

void CyclicEepromAdress_AdressIsWithinEepromsSize_AdressIsUnchanged()
{
    Controller controller = SetUpController();
    uint16_t expectedAdress = static_cast<uint16_t>(controller.storageSize - 13);

    uint16_t result = controller.CyclicEepromAdress(expectedAdress);

    ASSERT_TEST(result == expectedAdress);
}

void CalculateUnusedStorage_CalculatesTheAmountOfUnusedStorageAfterTheoreticallySavingTheGivenAmountOfKeymaps()
{
    Controller controller = SetUpController();
    uint8_t data = 0;
    DataPacket emptyPacket(&data, 0);
    const uint16_t sizeOfEmptyDataPacket = SizeOfSerializedDataPacket(emptyPacket),
             amountOfKeymaps = static_cast<uint16_t>(controller.customKeyMaps.length + 3),
             expectedAmountOfKeysInCustomKeymaps = static_cast<uint16_t>(amountOfKeymaps * controller.normalKeyCount),
             expectedSizeOfPayload = static_cast<uint16_t>(sizeof(BareKeyboardKey) * expectedAmountOfKeysInCustomKeymaps);
    int32_t expectedAmountOfUnusedStorage = static_cast<int32_t>(
        controller.storageSize - (
            sizeOfEmptyDataPacket +
            expectedSizeOfPayload
        )
    );

    int32_t result = controller.CalculateUnusedStorage(amountOfKeymaps);

    ASSERT_TEST(result == expectedAmountOfUnusedStorage);
}

void CalculateUnusedStorage_NoArguments_CalculatesTheUnusedStorageAsIfWeWereToSaveDownTheCustomKeyMaps()
{
    BareKeyboardKey keymap[genericNormalKeyCount] {
        BareKeyboardKey(9, 9),
        BareKeyboardKey(8, 8),
        BareKeyboardKey(7, 7),
        BareKeyboardKey(6, 6),
    };
    Controller controller = SetUpController();
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap);
    uint8_t data = 0;
    DataPacket emptyPacket(&data, 0);
    const uint16_t sizeOfEmptyDataPacket = SizeOfSerializedDataPacket(emptyPacket),
             amountOfKeymaps = static_cast<uint16_t>(controller.customKeyMaps.length),
             expectedAmountOfKeysInCustomKeymaps = static_cast<uint16_t>(amountOfKeymaps * controller.normalKeyCount),
             expectedSizeOfPayload = static_cast<uint16_t>(sizeof(BareKeyboardKey) * expectedAmountOfKeysInCustomKeymaps);
    int32_t expectedAmountOfUnusedStorage = static_cast<int32_t>(
        controller.storageSize - (
            sizeOfEmptyDataPacket +
            expectedSizeOfPayload
        )
    );

    int32_t result = controller.CalculateUnusedStorage();

    ASSERT_TEST(result == expectedAmountOfUnusedStorage);
}

void CalculateUnusedStorage_NoArguments_ReturnsTheSameSizeAsIfWeWereToSaveKeymapsToMemory()
{
    BareKeyboardKey keymap[genericNormalKeyCount] {
        BareKeyboardKey(9, 9),
        BareKeyboardKey(8, 8),
        BareKeyboardKey(7, 7),
        BareKeyboardKey(6, 6),
    };
    Controller controller = SetUpController();
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap);
    // Prepare to save packet
    DataPacket packet = DataToPacket(keymap);
    uint16_t sizeOfPacket = SizeOfSerializedDataPacket(packet);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, packet.payload, packet.payloadLength, controller.storageSize);
    int32_t expectedAmountOfUnusedStorage = static_cast<int32_t>(controller.storageSize - sizeOfPacket);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);
    int32_t result = controller.CalculateUnusedStorage();

    ASSERT_TEST(
        resultBool == true &&
        result == expectedAmountOfUnusedStorage
    );
}

void CalculateUnusedStorage_TooManyKeymapsToFitOnTheEeprom_ReturnsTheOverloadAsANegativeNumber()
{
    Controller controller = SetUpController();
    DataPacket emptyPacket;
    uint16_t sizeOfEmptyDataPacket = SizeOfSerializedDataPacket(emptyPacket),
             sizeOfKeymap = static_cast<uint16_t>(sizeof(BareKeyboardKey) * controller.normalKeyCount);
    uint16_t amountOfKeymaps = static_cast<uint16_t>(
        ((controller.storageSize - sizeOfEmptyDataPacket) / sizeOfKeymap) + 1
    );
    int32_t expectedAmountOfOverflow = static_cast<int32_t>(
        ((controller.storageSize - sizeOfEmptyDataPacket) % sizeOfKeymap) - sizeOfKeymap
    );

    int32_t result = controller.CalculateUnusedStorage(amountOfKeymaps);

    ASSERT_TEST(result == expectedAmountOfOverflow);
}

void RetrieveBareKeyboardKeysFromMemory_RetrievesFaultyData_DoesNotCrashAndReturnsFalse()
{
    Controller controller = SetUpController();
    // Set up packet with faulty payload
    BareKeyboardKey templateData[genericNormalKeyCount];
    const int templateDataSize = sizeof(templateData);
    uint8_t *dataPtr = new uint8_t[templateDataSize]{0};
    DataPacket packet(dataPtr, templateDataSize);
    // Mock function behaviour
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, packet, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(0, packet, eepromSize);

    BareKeyboardKey *result = new BareKeyboardKey[genericNormalKeyCount];
    uint16_t amountOfKeys, packetAdress, packetSize;
    bool doesNotCrash = false;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(&result, &amountOfKeys, &packetAdress, &packetSize);
    doesNotCrash = true;

    ASSERT_TEST(resultBool == false && doesNotCrash == true); // Test if working
    delete[](result);
    delete[](dataPtr);
}

void RetrieveBareKeyboardKeysFromMemory_FindsPacketAndReturnsTheBareKeyboardKeysInside()
{
    Controller controller = SetUpController();
    // Set up packet
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 8),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(4, 1),
        BareKeyboardKey(5, 9),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t expectedPacketSize = SizeOfSerializedDataPacket(packet);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, packet, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(0, packet, eepromSize);

    BareKeyboardKey *result = new BareKeyboardKey[genericNormalKeyCount];
    uint16_t amountOfKeys, packetAdress, packetSize;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(&result, &amountOfKeys, &packetAdress, &packetSize);

    ASSERT_TEST(
        resultBool == true &&
        result[0] == data[0] &&
        result[1] == data[1] &&
        result[2] == data[2] &&
        result[3] == data[3] &&
        amountOfKeys == genericNormalKeyCount &&
        packetAdress == 0 &&
        packetSize == expectedPacketSize
    );
    delete[](result);
}

void RetrieveBareKeyboardKeysFromMemory_FindsDefectPacket_ReturnsFalse()
{
    Controller controller = SetUpController();
    // Set up defect Packet
    BareKeyboardKey defectKey1(123, 2), defectKey2(111, 4);
    BareKeyboardKey defectData[2] = {
        defectKey1, defectKey2,
    };
    DataPacket defectPacket = DataToPacket(defectData);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, defectPacket, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(0, defectPacket, eepromSize);

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
    BareKeyboardKey defectKey1(169, 2), defectKey2(222, 4);
    BareKeyboardKey defectData[2] = {
        defectKey1, defectKey2,
    };
    DataPacket defectPacket = DataToPacket(defectData);
    // Set up valid packet
    BareKeyboardKey validKey1(controller.defaultKeymap[0].pin, 2), 
                    validKey2(controller.defaultKeymap[1].pin, 4);
    BareKeyboardKey validData[2] = {
        validKey1, validKey2,
    };
    DataPacket validPacket = DataToPacket(validData);
    // Prepare packets to be returned.
    uint16_t expectedDefectPacketSize = SizeOfSerializedDataPacket(defectPacket),
             expectedValidPacketSize = SizeOfSerializedDataPacket(validPacket);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, defectPacket, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(0, defectPacket, eepromSize);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedDefectPacketSize, validPacket, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(expectedDefectPacketSize, validPacket, eepromSize);

    BareKeyboardKey *result = new BareKeyboardKey[controller.normalKeyCount];
    uint16_t amountOfKeys, packetAdress, packetSize;
    bool resultBool = controller.RetrieveBareKeyboardKeysFromMemory(&result, &amountOfKeys, &packetAdress, &packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetAdress == expectedDefectPacketSize &&
        packetSize == expectedValidPacketSize &&
        amountOfKeys == 2 &&
        result[0] == validKey1 &&
        result[1] == validKey2 
    );
    delete[](result);
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

void InsertKeymapsFromStorage_CorrectlyLoadsKeymapIntoList()
{
    Controller controller = SetUpController();
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 0;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.InsertKeymapsFromStorage(&resultingKeymaps);
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

void InsertKeymapsFromStorage_CorrectlyLoadsKeymapIntoList_ReturnsTrue()
{
    Controller controller = SetUpController();
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 0;
    uint16_t eepromSize = controller.storageSize;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.InsertKeymapsFromStorage(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (!resultingKeymaps.IsEmpty())
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        resultBool == true &&
        resultingKeymaps.IsEmpty() == false &&
        result[0] == data[0] &&
        result[1] == data[1] &&
        result[2] == data[2] && 
        result[3] == data[3]
    );
}

void InsertKeymapsFromStorage_LoadsKeymap_SetsCurrentPacketAdressToTheLoadedPacketsAdress()
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
             expectedCurrentPacketAdress = packetAdress,
             eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(9);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.InsertKeymapsFromStorage(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (resultingKeymaps.IsEmpty() == false)
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        controller.currentPacketAdress == expectedCurrentPacketAdress &&
        resultingKeymaps.IsEmpty() == false
    );
}

void InsertKeymapsFromStorage_LoadsKeymap_SetsNextPacketAdressToAFreeAdressAfterTheEndOfTheLoadedPacket()
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
             expectedNextPacketAdress = static_cast<uint16_t>(packetAdress + SizeOfSerializedDataPacket(packet)),
             eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(9);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.InsertKeymapsFromStorage(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (resultingKeymaps.IsEmpty() == false)
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        controller.nextPacketAdress == expectedNextPacketAdress &&
        resultingKeymaps.IsEmpty() == false
    );
}

void InsertKeymapsFromStorage_LoadsKeymap_SetsTheAmountOfFreeStorageToBeEqualToTheStorageSizeMinusTheSizeOfTheLoadedPacket()
{
    Controller controller = SetUpController();
    controller.amountOfFreeStorage = 0;
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 0;
    uint16_t expectedAmountOfFreeStorage = static_cast<uint16_t>(controller.storageSize - SizeOfSerializedDataPacket(packet));
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps;
    controller.InsertKeymapsFromStorage(&resultingKeymaps);

    ASSERT_TEST(
        controller.amountOfFreeStorage == expectedAmountOfFreeStorage &&
        resultingKeymaps.IsEmpty() == false
    );
}

void InsertKeymapsFromStorage_GoesThroughWholeStorageWithoutFindingValidKeymaps_DoesNotRepeatInInfiniteLoop_AndReturnsFalse()
{
    BareKeyboardKey data1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(99, 7),
    };
    BareKeyboardKey data2[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(99, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet1 = DataToPacket(data1);
    DataPacket packet2 = DataToPacket(data2);
    uint16_t packetSize1 = SizeOfSerializedDataPacket(packet1),
             packetSize2 = SizeOfSerializedDataPacket(packet2),
             packetAdress1 = 0,
             packetAdress2 = packetSize1;
    uint16_t eepromSize = static_cast<uint16_t>(
        packetSize1 + packetSize2
    );
    EEPROMClass_length_return = eepromSize;
    const int normalKeyCount = 4;
    BareKeyboardKey defaultKeymap[normalKeyCount] = {
        BareKeyboardKey(2, 1),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(4, 3),
        BareKeyboardKey(5, 4),
    };
    const int specialKeyCount = 1;
    SpecialKey specialKeys[specialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    Controller controller(defaultKeymap, normalKeyCount, specialKeys, specialKeyCount);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress1, packet1, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress1, packet1, controller.storageSize);
    // Push back garbage so we can start reading packet2 from the right adress for the test to work.
    for(uint16_t i = 0; i < (packetSize1 - 1); i++)
    {
        EEPROMClass_read_return_v.push_back(0);
    }
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress2, packet2, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress2, packet2, controller.storageSize);
    // Push back garbage so we can start reading packet1 from the beginning of the eeprom again.
    for(uint16_t i = 0; i < (packetSize2 - 1); i++)
    {
        EEPROMClass_read_return_v.push_back(0);
    }
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress1, packet1, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress1, packet1, controller.storageSize);
    for(uint16_t i = 0; i < (packetSize1 - 1); i++)
    {
        EEPROMClass_read_return_v.push_back(0);
    }
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress2, packet2, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress2, packet2, controller.storageSize);
    for(uint16_t i = 0; i < (packetSize2 - 1); i++)
    {
        EEPROMClass_read_return_v.push_back(0);
    }

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.InsertKeymapsFromStorage(&resultingKeymaps);

    ASSERT_TEST(
        resultBool == false &&
        resultingKeymaps.IsEmpty() == true &&
        EEPROMClass_get_invocations_o3 <= 3
    );
}

void InsertKeymapsFromStorage_OnlyOneInvalidKeymapInMemory_DoesNotAttemptToLoadTheSamePacketTwice_AndReturnsFalse()
{
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(99, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetSize = SizeOfSerializedDataPacket(packet),
             packetAdress = 0;
    uint16_t eepromSize = packetSize;
    EEPROMClass_length_return = eepromSize;
    const int normalKeyCount = 4;
    BareKeyboardKey defaultKeymap[normalKeyCount] = {
        BareKeyboardKey(2, 1),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(4, 3),
        BareKeyboardKey(5, 4),
    };
    const int specialKeyCount = 1;
    SpecialKey specialKeys[specialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    Controller controller(defaultKeymap, normalKeyCount, specialKeys, specialKeyCount);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);
    // Push back garbage so we can start reading packet from the beginning of the eeprom again.
    for(uint16_t i = 0; i < (packetSize - 1); i++)
    {
        EEPROMClass_read_return_v.push_back(0);
    }
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);
    for(uint16_t i = 0; i < (packetSize - 1); i++)
    {
        EEPROMClass_read_return_v.push_back(0);
    }
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);
    for(uint16_t i = 0; i < (packetSize - 1); i++)
    {
        EEPROMClass_read_return_v.push_back(0);
    }

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.InsertKeymapsFromStorage(&resultingKeymaps);

    ASSERT_TEST(
        resultBool == false &&
        resultingKeymaps.IsEmpty() == true &&
        EEPROMClass_get_invocations_o3 <= 2
    );
}

void InsertKeymapsFromStorage_FailsToLoadKeymap_DoesNotChangeAmountOfFreeStorage()
{
    Controller controller = SetUpController();
    controller.amountOfFreeStorage = 730;
    EEPROMClass_length_return = controller.storageSize;
    uint16_t amountOfFreeStorageBefore = controller.amountOfFreeStorage;

    LinkedList<BareKeyboardKey *> resultingKeymaps;
    controller.InsertKeymapsFromStorage(&resultingKeymaps);
    uint16_t amountOfFreeStorageAfter = controller.amountOfFreeStorage;

    ASSERT_TEST(
        amountOfFreeStorageBefore == amountOfFreeStorageAfter &&
        resultingKeymaps.IsEmpty() == true
    );
}

void InsertKeymapsFromStorage_NoPacketInStorage_ReturnsFalse()
{
    Controller controller = SetUpController();

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.InsertKeymapsFromStorage(&resultingKeymaps);

    ASSERT_TEST(
        resultBool == false
    );
}

void InsertKeymapsFromStorage_LoadsKeymapThatExceedsTheEepromsLastAdress_DoesNotUpdateNextPacketAdressWithAnAdressOutsideTheEeprom()
{
    uint16_t eepromSize = 60;
    EEPROMClass_length_return = eepromSize;
    const int normalKeyCount = 4;
    BareKeyboardKey defaultKeymap[normalKeyCount] = {
        BareKeyboardKey(2, 1),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(4, 3),
        BareKeyboardKey(5, 4),
    };
    const int specialKeyCount = 1;
    SpecialKey specialKeys[specialKeyCount] {
        SpecialKey(12, toggleDefaultKeyMap),
    };
    Controller controller(defaultKeymap, normalKeyCount, specialKeys, specialKeyCount);
    controller.nextPacketAdress = 0;
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetSize = SizeOfSerializedDataPacket(packet),
             packetAdress = static_cast<uint16_t>(controller.storageSize - (packetSize / 2)),
             expectedNextPacketAdress = CyclicAdress(packetAdress + packetSize, controller.storageSize);
    // Push garbage so that the packet will be mocked at the right adress.
    for(int i = 0; i < packetAdress; i++)
    {
        EEPROMClass_read_return_v.push_back(13);
    }
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(packetAdress, packet, controller.storageSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.InsertKeymapsFromStorage(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (resultingKeymaps.IsEmpty() == false)
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        controller.nextPacketAdress == expectedNextPacketAdress &&
        resultingKeymaps.IsEmpty() == false
    );
}

void InsertKeymapsFromStorage_EepromHasDefectKeymaps_DoesNotLoadKeymaps()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
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
    BareKeyboardKey data[normalKeyCount] = {
        BareKeyboardKey(111, 4), 
        BareKeyboardKey(24, 26), 
        BareKeyboardKey(91, 22), 
        BareKeyboardKey(87, 7)
    };
    DataPacket packet = DataToPacket(data);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, packet, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(0, packet, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.InsertKeymapsFromStorage(&resultingKeymaps);

    ASSERT_TEST(resultingKeymaps.IsEmpty());
}

void InsertKeymapsFromStorage_EepromHasDefectKeymapsFollowedByValidKeymaps_LoadsTheValidKeymaps()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
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
    BareKeyboardKey defectData[normalKeyCount] = {
        BareKeyboardKey(140, 4),
        BareKeyboardKey(75, 26),
        BareKeyboardKey(86, 22),
        BareKeyboardKey(188, 7),
    };
    BareKeyboardKey validData[normalKeyCount] = {
        BareKeyboardKey(2, 4),
        BareKeyboardKey(3, 26),
        BareKeyboardKey(4, 22),
        BareKeyboardKey(5, 7),
    };
    // Set up valid packet.
    DataPacket validPacket = DataToPacket(validData);
    // Set up defect packet.
    DataPacket defectPacket = DataToPacket(defectData);
    uint16_t validPacketAdress = SizeOfSerializedDataPacket(defectPacket);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, defectPacket, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(0, defectPacket, eepromSize);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(validPacketAdress, validPacket, eepromSize);
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(validPacketAdress, validPacket, eepromSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.InsertKeymapsFromStorage(&resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (!resultingKeymaps.IsEmpty())
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        resultingKeymaps.IsEmpty() == false &&
        result[0] == validData[0] &&
        result[1] == validData[1] &&
        result[2] == validData[2] && 
        result[3] == validData[3]
    );
}

void AddKeymapsFromPayloadIntoList_SuccessfullyAddsKeymapToList()
{
    Controller controller = SetUpController();
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 0;
    uint16_t payloadAdress = static_cast<uint16_t>(
        packetAdress +
        SizeOfEmptySerializedDataPacket()
        - sizeof(packet.etx)
    );
    EEPROMClass_length_return = controller.storageSize;
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    controller.AddKeymapsFromPayloadIntoList(payloadAdress, packet.payloadLength, &resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (!resultingKeymaps.IsEmpty())
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        resultingKeymaps.IsEmpty() == false &&
        result[0] == data[0] &&
        result[1] == data[1] &&
        result[2] == data[2] && 
        result[3] == data[3]
    );
}

void AddKeymapsFromPayloadIntoList_AddsKeymapToList_ReturnsTrue()
{
    Controller controller = SetUpController();
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 0;
    uint16_t payloadAdress = static_cast<uint16_t>(
        packetAdress +
        SizeOfEmptySerializedDataPacket()
        - sizeof(packet.etx)
    );
    EEPROMClass_length_return = controller.storageSize;
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.AddKeymapsFromPayloadIntoList(payloadAdress, packet.payloadLength, &resultingKeymaps);
    BareKeyboardKey *result = nullptr;
    if (!resultingKeymaps.IsEmpty())
    {
        result = *(resultingKeymaps[0]);
    }

    ASSERT_TEST(
        resultingKeymaps.IsEmpty() == false &&
        resultBool == true
    );
}

void AddKeymapsFromPayloadIntoList_SuccessfullyAddsMultipleKeymapsToList()
{
    Controller controller = SetUpController();
    const uint16_t amountOfkeysInData = 2 * genericNormalKeyCount;
    BareKeyboardKey data[amountOfkeysInData] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),

        BareKeyboardKey(2, 1), 
        BareKeyboardKey(3, 2), 
        BareKeyboardKey(4, 3), 
        BareKeyboardKey(5, 4),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 0;
    uint16_t payloadAdress = static_cast<uint16_t>(
        packetAdress +
        SizeOfEmptySerializedDataPacket()
        - sizeof(packet.etx)
    );
    EEPROMClass_length_return = controller.storageSize;
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.AddKeymapsFromPayloadIntoList(payloadAdress, packet.payloadLength, &resultingKeymaps);
    BareKeyboardKey *result1 = nullptr, *result2 = nullptr;
    bool isResultSafeToAccess = false;
    if (resultingKeymaps.length >= 2)
    {
        result1 = *(resultingKeymaps[0]);
        result2 = *(resultingKeymaps[1]);
        isResultSafeToAccess = true;
    }

    ASSERT_TEST(
        resultingKeymaps.IsEmpty() == false &&
        resultBool == true &&

        isResultSafeToAccess &&
        result1[0] == data[0] &&
        result1[1] == data[1] &&
        result1[2] == data[2] && 
        result1[3] == data[3] &&

        result2[0] == data[4] &&
        result2[1] == data[5] &&
        result2[2] == data[6] && 
        result2[3] == data[7]
    );
}

void AddKeymapsFromPayloadIntoList_OneKeymapFails_RemovesKeymapsThatWereAddedFromFaultyPayload()
{
    Controller controller = SetUpController();
    const uint16_t amountOfkeysInData = 3 * genericNormalKeyCount;
    BareKeyboardKey data[amountOfkeysInData] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),

        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(5, 7),

        BareKeyboardKey(2, 1), 
        BareKeyboardKey(99, 2), 
        BareKeyboardKey(4, 3), 
        BareKeyboardKey(5, 4),
    };
    BareKeyboardKey initialKeymap[genericNormalKeyCount] = {
        BareKeyboardKey(2, 55),
        BareKeyboardKey(3, 55),
        BareKeyboardKey(4, 55),
        BareKeyboardKey(5, 55),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 0;
    uint16_t payloadAdress = static_cast<uint16_t>(
        packetAdress +
        SizeOfEmptySerializedDataPacket()
        - sizeof(packet.etx)
    );
    EEPROMClass_length_return = controller.storageSize;
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    resultingKeymaps.Add(initialKeymap);
    bool resultBool = controller.AddKeymapsFromPayloadIntoList(payloadAdress, packet.payloadLength, &resultingKeymaps);
    BareKeyboardKey **remainingKeymap = resultingKeymaps[0];

    ASSERT_TEST(
        resultingKeymaps.length == 1 &&
        (*remainingKeymap)[0] == initialKeymap[0] &&
        (*remainingKeymap)[1] == initialKeymap[1] &&
        (*remainingKeymap)[2] == initialKeymap[2] &&
        (*remainingKeymap)[3] == initialKeymap[3] &&
        resultBool == false    
    );
}

void AddKeymapsFromPayloadIntoList_FailsToReadPayload_ReturnsFalse()
{
    Controller controller = SetUpController();
    uint16_t payloadLength = sizeof(BareKeyboardKey) * genericNormalKeyCount;
    EEPROMClass_length_return = controller.storageSize;

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.AddKeymapsFromPayloadIntoList(0, payloadLength, &resultingKeymaps);

    ASSERT_TEST(
        resultBool == false
    );
}

void AddKeymapsFromPayloadIntoList_KeymapInPayloadIsInvalid_ReturnsFalse()
{
    Controller controller = SetUpController();
    BareKeyboardKey data[genericNormalKeyCount] = {
        BareKeyboardKey(2, 4), 
        BareKeyboardKey(3, 26), 
        BareKeyboardKey(4, 22), 
        BareKeyboardKey(99, 7),
    };
    DataPacket packet = DataToPacket(data);
    uint16_t packetAdress = 0;
    uint16_t payloadAdress = static_cast<uint16_t>(
        packetAdress +
        SizeOfEmptySerializedDataPacket()
        - sizeof(packet.etx)
    );
    EEPROMClass_length_return = controller.storageSize;
    Helper_ReadBytesFromEEPROM_PreparesToReadPayload(packetAdress, packet, controller.storageSize);

    LinkedList<BareKeyboardKey *> resultingKeymaps = LinkedList<BareKeyboardKey *>();
    bool resultBool = controller.AddKeymapsFromPayloadIntoList(payloadAdress, packet.payloadLength, &resultingKeymaps);

    ASSERT_TEST(
        resultBool == false &&
        resultingKeymaps.IsEmpty() == true
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
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(controller.currentPacketAdress, expectedPacket, controller.storageSize);

    controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    bool success = true;
    if(EEPROMClass_update_invocations >= expectedPacket.payloadLength)
    {
        for (uint16_t i = 0; i < expectedPacket.payloadLength; i++)
        {
            if (EEPROMClass_update_param_val_v[i] != expectedPacket.payload[i])
            {
                success = false;
            } 
        }
    } else
    {
        success = false;
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
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(controller.currentPacketAdress, expectedPacket, controller.storageSize);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    bool success = true;
    if(EEPROMClass_update_invocations >= expectedPacket.payloadLength)
    {
        for (uint16_t i = 0; i < expectedPacket.payloadLength; i++)
        {
            if (EEPROMClass_update_param_val_v[i] != expectedPacket.payload[i])
            {
                success = false;
            } 
        }
    } else
    {
        success = false;
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
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedCurrentAdress, packet, controller.storageSize);

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
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adressToSavePacket, packet, controller.storageSize);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    ASSERT_TEST(
        resultBool == true &&
        controller.nextPacketAdress == expectedNextPacketAdress
    );
}

void SaveKeyMapsToMemory_SavesKeymaps_SetsTheAmountOfFreeStorageToBeEqualToTheStorageSizeMinusTheSizeOfTheSavedPacket()
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
    unsigned int packetSize = SizeOfSerializedDataPacket(packet);
    uint16_t expectedAmountOfFreeStorage = static_cast<uint16_t>(controller.storageSize - packetSize);
    // Setup mocked packet to return so that the function succeeds.
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, packet, controller.storageSize);

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);

    ASSERT_TEST(
        resultBool == true &&
        controller.amountOfFreeStorage == expectedAmountOfFreeStorage
    );
}

void SaveKeyMapsToMemory_FailsToSavePacket_DoesNotChangeAmountOfFreeStorage()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] = {
        BareKeyboardKey(2, 0),
        BareKeyboardKey(3, 1),
        BareKeyboardKey(4, 2),
        BareKeyboardKey(5, 3),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    uint16_t amountOfFreeStorageBefore = controller.amountOfFreeStorage;

    bool resultBool = controller.SaveKeyMapsToMemory(controller.customKeyMaps);
    uint16_t amountOfFreeStorageAfter = controller.amountOfFreeStorage;

    ASSERT_TEST(
        resultBool == false &&
        amountOfFreeStorageBefore == amountOfFreeStorageAfter
    );
}

void SaveKeyMapsToMemory_SavedPacketExceedsTheLastAdressOfTheEeprom_DoesNotSetNextPacketAdressToAnAdressOutsideTheEeprom()
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
    uint16_t packetSize = Helper_CalculateSizeOfPacketOnEEPROM(packet);
    uint16_t adressToSavePacket = static_cast<uint16_t>(controller.storageSize - (packetSize / 2));
    uint16_t expectedNextPacketAdress = CyclicAdress(adressToSavePacket + packetSize, controller.storageSize);
    controller.currentPacketAdress = 0;
    controller.nextPacketAdress = adressToSavePacket;
    // Setup mocked packet to return so that the function succeeds.
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adressToSavePacket, packet, controller.storageSize);

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
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress, packet, controller.storageSize);

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
    uint16_t eepromSize = 1024;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(controller.nextPacketAdress, packet, eepromSize);
    EEPROMClass_length_return = eepromSize;
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
    EEPROMClass_length_return = controller.storageSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(controller.nextPacketAdress, packet, controller.storageSize);
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

void CreateNewKeymap_SuccessfullyCreatesAKeymapInCustomKeymaps()
{
    Controller controller = SetUpController();
    unsigned int lengthBefore = controller.customKeyMaps.length;

    controller.CreateNewKeymap();
    unsigned int lengthAfter = controller.customKeyMaps.length;

    ASSERT_TEST(
        lengthAfter == (lengthBefore + 1)
    );
}

void CreateNewKeymap_SuccessfullyCreatesAKeymap_ReturnsTrue()
{
    Controller controller = SetUpController();
    unsigned int lengthBefore = controller.customKeyMaps.length;

    bool resultBool = controller.CreateNewKeymap();
    unsigned int lengthAfter = controller.customKeyMaps.length;

    ASSERT_TEST(
        lengthAfter == (lengthBefore + 1) &&
        resultBool == true
    );
}

void CreateNewKeymap_SuccessfullyCreatesAKeymap_KeymapIsAddedToTheEndOfTheCustomKeymapList()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] {
        BareKeyboardKey(93, 1),
        BareKeyboardKey(83, 2),
        BareKeyboardKey(93, 3),
        BareKeyboardKey(45, 0),
    };
    BareKeyboardKey keymap2[genericNormalKeyCount] {
        BareKeyboardKey(8, 1),
        BareKeyboardKey(8, 2),
        BareKeyboardKey(5, 3),
        BareKeyboardKey(13, 0),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);
    controller.customKeyMaps.Add(keymap2);
    
    bool resultBool = controller.CreateNewKeymap();
    unsigned int lastKeymapIndex = controller.customKeyMaps.length - 1;
    BareKeyboardKey *lastKeymap = *controller.customKeyMaps[lastKeymapIndex];

    ASSERT_TEST(
        controller.customKeyMaps.length == 3 &&
        resultBool == true &&

        // Asserts that the last keymap in the custom keymaps list is not one of the old keymaps.
        // Therefore the last keymap must be the newly added keymap.
        (
            lastKeymap[0] != keymap1[0] ||
            lastKeymap[1] != keymap1[1] ||
            lastKeymap[2] != keymap1[2] ||
            lastKeymap[3] != keymap1[3]
        ) 
        && 
        (
            lastKeymap[0] != keymap2[0] ||
            lastKeymap[1] != keymap2[1] ||
            lastKeymap[2] != keymap2[2] ||
            lastKeymap[3] != keymap2[3]
        )
    );
}

void CreateNewKeymap_SuccessfullyCreatesAKeymap_NewKeymapInheritsPinsFromDefaultKeymap()
{
    const int amountOfDefaultKeys = 4;
    BareKeyboardKey defaultKeymap[amountOfDefaultKeys] {
        BareKeyboardKey(9, 1),
        BareKeyboardKey(3, 2),
        BareKeyboardKey(3, 3),
        BareKeyboardKey(5, 4),
    };
    const int amountOfSpecialKeys = 1;
    SpecialKey specialKeymap[amountOfSpecialKeys] {
        SpecialKey(10, toggleDefaultKeyMap),
    };
    EEPROMClass_length_return = 1024;
    uint16_t amountOfFreeSRAM = 2048;
    freeMemory_return = amountOfFreeSRAM;
    Controller controller = Controller(defaultKeymap, amountOfDefaultKeys, specialKeymap, amountOfSpecialKeys);

    bool resultBool = controller.CreateNewKeymap();
    BareKeyboardKey *newlyAddedKeymap = *controller.customKeyMaps[controller.customKeyMaps.length - 1];

    ASSERT_TEST(
        newlyAddedKeymap[0].pin == controller.defaultKeymap[0].pin &&
        newlyAddedKeymap[1].pin == controller.defaultKeymap[1].pin &&
        newlyAddedKeymap[2].pin == controller.defaultKeymap[2].pin &&
        newlyAddedKeymap[3].pin == controller.defaultKeymap[3].pin &&
        resultBool == true
    );
}

void CreateNewKeymap_SuccessfullyCreatesAKeymap_KeycodesOnNewKeymapsDefaultTo4()
{
    Controller controller = SetUpController();

    bool resultBool = controller.CreateNewKeymap();
    BareKeyboardKey *newlyAddedKeymap = *controller.customKeyMaps[controller.customKeyMaps.length - 1];

    ASSERT_TEST(
        newlyAddedKeymap[0].keyCode == 4 &&
        newlyAddedKeymap[1].keyCode == 4 &&
        newlyAddedKeymap[2].keyCode == 4 &&
        newlyAddedKeymap[3].keyCode == 4 &&
        resultBool == true
    );
}

void CreateNewKeymap_SuccessfullyCreatesAKeymap_EquipsTheNewKeymap()
{
    Controller controller = SetUpController();
    BareKeyboardKey keymap1[genericNormalKeyCount] {
        BareKeyboardKey(93, 1),
        BareKeyboardKey(83, 2),
        BareKeyboardKey(93, 3),
        BareKeyboardKey(45, 0),
    };
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap1);

    bool resultBool = controller.CreateNewKeymap();
    unsigned int expectedKeymapIndex = controller.customKeyMaps.length - 1;
    BareKeyboardKey *newKeymap = *controller.customKeyMaps[expectedKeymapIndex];
    Key *equippedKeymap = controller.currentKeyMap;

    ASSERT_TEST(
        equippedKeymap[0].pin == newKeymap[0].pin && equippedKeymap[0].keyCode == newKeymap[0].keyCode &&
        equippedKeymap[1].pin == newKeymap[1].pin && equippedKeymap[1].keyCode == newKeymap[1].keyCode &&
        equippedKeymap[2].pin == newKeymap[2].pin && equippedKeymap[2].keyCode == newKeymap[2].keyCode &&
        equippedKeymap[3].pin == newKeymap[3].pin && equippedKeymap[3].keyCode == newKeymap[3].keyCode &&
        controller.customKeyMapIndex == expectedKeymapIndex &&
        resultBool == true
    );
}

void CreateNewKeymap_WeHaveEnoughStorageSpace_CreatesKeymapAndReturnsTrue()
{
    const int amountOfDefaultKeys = 2;
    BareKeyboardKey defaultKeymap[amountOfDefaultKeys] {
        BareKeyboardKey(9, 1), 
        BareKeyboardKey(3, 2),
    };
    const int amountOfSpecialKeys = 1;
    SpecialKey specialKeymap[amountOfSpecialKeys] {
        SpecialKey(10, toggleDefaultKeyMap),
    };
    BareKeyboardKey keymap[amountOfDefaultKeys] {
        BareKeyboardKey(1, 10), 
        BareKeyboardKey(2, 20),
    };
    uint16_t eepromSize = static_cast<uint16_t>(
        sizeof(keymap) * 2 + SizeOfSerializedDataPacket(DataPacket())
    );
    EEPROMClass_length_return = eepromSize;
    uint16_t amountOfFreeSRAM = 2048;
    freeMemory_return = amountOfFreeSRAM;
    Controller controller = Controller(defaultKeymap, amountOfDefaultKeys, specialKeymap, amountOfSpecialKeys);
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap);
    uint16_t customKeyMapLengthBefore = static_cast<uint16_t>(controller.customKeyMaps.length);

    bool resultBool = controller.CreateNewKeymap();

    ASSERT_TEST(
        resultBool == true &&
        controller.customKeyMaps.length == static_cast<uint16_t>(customKeyMapLengthBefore + 1)
    );
}

void CreateNewKeymap_WeHaveSufficientFreeSram_CreatesKeymapAndReturnsTrue()
{
    const int amountOfDefaultKeys = 2;
    BareKeyboardKey defaultKeymap[amountOfDefaultKeys] {
        BareKeyboardKey(9, 1),
        BareKeyboardKey(3, 2),
    };
    const int amountOfSpecialKeys = 1;
    SpecialKey specialKeymap[amountOfSpecialKeys] {
        SpecialKey(10, toggleDefaultKeyMap),
    };
    BareKeyboardKey keymap[amountOfDefaultKeys] {
        BareKeyboardKey(1, 10),
        BareKeyboardKey(2, 20),
    };
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Controller controller = Controller(defaultKeymap, amountOfDefaultKeys, specialKeymap, amountOfSpecialKeys);
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap);
    controller.ChangeKeyMap(keymap);
    uint16_t amountOfFreeSRAM = static_cast<uint16_t>(controller.SRAMSafetyThreshold + sizeof(keymap));
    freeMemory_return = amountOfFreeSRAM;
    uint16_t customKeyMapLengthBefore = static_cast<uint16_t>(controller.customKeyMaps.length);

    bool resultBool = controller.CreateNewKeymap();

    ASSERT_TEST(
        resultBool == true &&
        controller.customKeyMaps.length == static_cast<uint16_t>(customKeyMapLengthBefore + 1)
    );
}

void CreateNewKeymap_EepromDoesNotFitAnotherKeymap_DoesNotCreateNorChangeKeymapAndReturnsFalse()
{
    const int amountOfDefaultKeys = 2;
    BareKeyboardKey defaultKeymap[amountOfDefaultKeys] {
        BareKeyboardKey(9, 1),
        BareKeyboardKey(3, 2),
    };
    const int amountOfSpecialKeys = 1;
    SpecialKey specialKeymap[amountOfSpecialKeys] {
        SpecialKey(10, toggleDefaultKeyMap),
    };
    BareKeyboardKey keymap[amountOfDefaultKeys] {
        BareKeyboardKey(1, 10),
        BareKeyboardKey(2, 20),
    };
    uint16_t eepromSize = static_cast<uint16_t>(
        sizeof(keymap) + SizeOfSerializedDataPacket(DataPacket())
    );
    EEPROMClass_length_return = eepromSize;
    uint16_t amountOfFreeSRAM = 2048;
    freeMemory_return = amountOfFreeSRAM;
    Controller controller = Controller(defaultKeymap, amountOfDefaultKeys, specialKeymap, amountOfSpecialKeys);
    controller.customKeyMaps.Clear();
    controller.ChangeKeyMap(keymap);
    controller.customKeyMaps.Add(keymap);
    uint16_t customKeyMapLengthBefore = static_cast<uint16_t>(controller.customKeyMaps.length);

    bool resultBool = controller.CreateNewKeymap();

    ASSERT_TEST(
        resultBool == false &&
        controller.customKeyMaps.length == customKeyMapLengthBefore &&
        controller.currentKeyMap[0].pin == keymap[0].pin && controller.currentKeyMap[0].keyCode == keymap[0].keyCode &&
        controller.currentKeyMap[1].pin == keymap[1].pin && controller.currentKeyMap[1].keyCode == keymap[1].keyCode
    );
}

void CreateNewKeymap_SramDoesNotFitAnotherKeymap_DoesNotCreateNorChangeKeymapAndReturnsFalse()
{
    const int amountOfDefaultKeys = 2;
    BareKeyboardKey defaultKeymap[amountOfDefaultKeys] {
        BareKeyboardKey(9, 1),
        BareKeyboardKey(3, 2),
    };
    const int amountOfSpecialKeys = 1;
    SpecialKey specialKeymap[amountOfSpecialKeys] {
        SpecialKey(10, toggleDefaultKeyMap),
    };
    BareKeyboardKey keymap[amountOfDefaultKeys] {
        BareKeyboardKey(1, 10),
        BareKeyboardKey(2, 20),
    };
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Controller controller = Controller(defaultKeymap, amountOfDefaultKeys, specialKeymap, amountOfSpecialKeys);
    uint16_t amountOfFreeSRAM = static_cast<uint16_t>(controller.SRAMSafetyThreshold - sizeof(keymap));
    freeMemory_return = amountOfFreeSRAM;
    controller.customKeyMaps.Clear();
    controller.customKeyMaps.Add(keymap);
    controller.ChangeKeyMap(keymap);
    uint16_t customKeyMapLengthBefore = static_cast<uint16_t>(controller.customKeyMaps.length);

    bool resultBool = controller.CreateNewKeymap();

    ASSERT_TEST(
        resultBool == false &&
        controller.customKeyMaps.length == customKeyMapLengthBefore &&
        controller.currentKeyMap[0].pin == keymap[0].pin && controller.currentKeyMap[0].keyCode == keymap[0].keyCode &&
        controller.currentKeyMap[1].pin == keymap[1].pin && controller.currentKeyMap[1].keyCode == keymap[1].keyCode
    );
}