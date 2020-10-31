#include "../test.h"
#include "TestHelper/DataPacketHelper.h"

void Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket_ParsePacketFromEepromSuccessfullyReturnsCorrectPacket() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ReadDataPacketOnEEPROM(0, &result, &packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packet.stx == result.stx &&
        packet.active == result.active &&
        packet.payloadLength == result.payloadLength &&
        packet.crc == result.crc &&
        packet.payload[0] == result.payload[0] &&
        packet.payload[1] == result.payload[1] &&
        packet.etx == result.etx &&
        Helper_CalculateSizeOfPacketOnEEPROM(packet) == packetSize
    );
}

void Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket_SetEepromSizeByHandToFitThePacket_ParsePacketFromEepromSuccessfullyReturnsCorrectPacket() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet, static_cast<uint16_t>(65535u));
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ReadDataPacketOnEEPROM(0, &result, &packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packet.stx == result.stx &&
        packet.active == result.active &&
        packet.payloadLength == result.payloadLength &&
        packet.crc == result.crc &&
        packet.payload[0] == result.payload[0] &&
        packet.payload[1] == result.payload[1] &&
        packet.etx == result.etx &&
        Helper_CalculateSizeOfPacketOnEEPROM(packet) == packetSize
    );
}

void Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket_UsingHighAdress_EepromLengthIsSetToFitPacket()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t adress = 10000;

    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(adress, packet);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ReadDataPacketOnEEPROM(adress, &result, &packetSize);

    ASSERT_TEST(resultBool == true);
}

void Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket_SetEepromSizeByHandToNotFitThePacket_ReadDataPacketOnEEPROMFails() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t eepromSize = 1;

    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet, eepromSize);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ReadDataPacketOnEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
}

void Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket_ParsePacketFromEepromRecievesInactiveFlag() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    packet.active = 0x00;

    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ReadDataPacketOnEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
}

void Helper_IsPacketValidOnEEPROM_PrepareToReadPacket_ReadsValidPacketFromEepromSuccessfullyAndReturnsTrueAndOutputsCorrectValues() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t expectedPayloadLength = packet.payloadLength;
    uint16_t expectedPacketSize = SizeOfSerializedDataPacket(packet);

    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, packet);
    uint16_t sizeOfPacket, adressOfPayload, lengthOfPayload;
    bool resultBool = IsPacketOnEEPROMValid(0, &sizeOfPacket, &adressOfPayload, &lengthOfPayload);

    ASSERT_TEST(
        resultBool == true && 
        lengthOfPayload == expectedPayloadLength &&
        sizeOfPacket == expectedPacketSize
    );
}

void Helper_IsPacketValidOnEEPROM_PrepareToReadPacket_SetEepromSizeByHandToFitThePacket_SuccessfullyValidatesThePacket() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, packet, static_cast<uint16_t>(65535u));
    uint16_t sizeOfPacket, adressOfPayload, lengthOfPayload;
    bool resultBool = IsPacketOnEEPROMValid(0, &sizeOfPacket, &adressOfPayload, &lengthOfPayload);

    ASSERT_TEST(resultBool == true);
}

void Helper_IsPacketValidOnEEPROM_PrepareToReadPacket_UsingHighAdress_EepromLengthIsSetToFitPacket()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t adress = 10000;

    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet);
    uint16_t sizeOfPacket, adressOfPayload, lengthOfPayload;
    bool resultBool = IsPacketOnEEPROMValid(adress, &sizeOfPacket, &adressOfPayload, &lengthOfPayload);

    ASSERT_TEST(resultBool == true);
}

void Helper_IsPacketValidOnEEPROM_PrepareToReadPacket_SetEepromSizeByHandToNotFitThePacket_ValidationOfPacketFails() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t eepromSize = 1;

    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, packet, eepromSize);
    uint16_t sizeOfPacket, adressOfPayload, lengthOfPayload;
    bool resultBool = IsPacketOnEEPROMValid(0, &sizeOfPacket, &adressOfPayload, &lengthOfPayload);

    ASSERT_TEST(resultBool == false);
}

void Helper_IsPacketValidOnEEPROM_PrepareToReadPacket_RecievesInactiveFlag_ReturnsFalse() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    packet.active = 0x00;

    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(0, packet);
    uint16_t sizeOfPacket, adressOfPayload, lengthOfPayload;
    bool resultBool = IsPacketOnEEPROMValid(0, &sizeOfPacket, &adressOfPayload, &lengthOfPayload);

    ASSERT_TEST(resultBool == false);
}

void Helper_SaveDataPacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_SaveDataPacketToEEPROMReturnsTrueAndReturnsCorrectPacketSize()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, dataPtr, sizeof(data));
    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(0, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetSize == Helper_CalculateSizeOfPacketOnEEPROM(packet)
    );
}

void Helper_SaveDataPacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_SetEepromSizeByHandToNotFitPacket_ReturnsFalse()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    uint16_t eepromSize = 1;

    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, dataPtr, sizeof(data), eepromSize);
    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(0, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == false);
}

void Helper_SaveDataPacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_SetEepromSizeByHandToFitPacket_ReturnsTrue()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t eepromSize = static_cast<uint16_t>(Helper_CalculateSizeOfPacketOnEEPROM(packet) + 10);

    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, dataPtr, sizeof(data), eepromSize);
    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(0, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == true);
}

void Helper_SaveDataPacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_WeProvideNoEepromSize_SetsEepromSizeAutomaticallyToFitOurPacket()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);

    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, dataPtr, sizeof(data));
    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(0, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == true);
}

void Helper_SaveDataPacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_UsingHighAdress_EepromLengthIsSetToFitPacket()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    uint16_t adress = 10000;

    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, dataPtr, sizeof(data));
    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == true);
}

void Helper_CalculateSizeOfPacketOnEEPROM_ReturnsCorrectSizeOfPacket()
{
    uint16_t data = 123;
    DataPacket packet = DataPacket(reinterpret_cast<uint8_t*>(&data), sizeof(data));
    uint16_t expectedSize = static_cast<uint16_t>(
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc) +
        sizeof(packet.payload[0]) * packet.payloadLength +
        sizeof(packet.etx)
    );

    uint16_t result = Helper_CalculateSizeOfPacketOnEEPROM(packet);

    ASSERT_TEST(result == expectedSize);
}
