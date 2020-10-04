#include "../test.h"
#include "TestHelper/DataPacketHelper.h"

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_ParsePacketFromEepromSuccessfullyReturnsCorrectPacket() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(0, packet);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);

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

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_SetEepromSizeByHandToFitThePacket_ParsePacketFromEepromSuccessfullyReturnsCorrectPacket() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(0, packet, static_cast<uint16_t>(65535u));
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);

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

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_UsingHighAdress_EepromLengthIsSetToFitPacket()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t adress = 10000;

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(adress, packet);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(adress, &result, &packetSize);

    ASSERT_TEST(resultBool == true);
}

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_SetEepromSizeByHandToNotFitThePacket_ParsePacketFromEEPROMFails() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t eepromSize = 1;

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(0, packet, eepromSize);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
}

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_ParsePacketFromEepromRecievesInactiveFlag() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    packet.active = 0x00;

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(0, packet);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
}

void Helper_SavePacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_SavePacketToEEPROMReturnsTrueAndReturnsCorrectPacketSize()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_SavePacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, dataPtr, sizeof(data));
    uint16_t packetSize;
    bool resultBool = SavePacketToEEPROM(0, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetSize == Helper_CalculateSizeOfPacketOnEEPROM(packet)
    );
}

void Helper_SavePacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_SetEepromSizeByHandToNotFitPacket_ReturnsFalse()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    uint16_t eepromSize = 1;

    Helper_SavePacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, dataPtr, sizeof(data), eepromSize);
    uint16_t packetSize;
    bool resultBool = SavePacketToEEPROM(0, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == false);
}

void Helper_SavePacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_SetEepromSizeByHandToFitPacket_ReturnsTrue()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t eepromSize = static_cast<uint16_t>(Helper_CalculateSizeOfPacketOnEEPROM(packet) + 10);

    Helper_SavePacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, dataPtr, sizeof(data), eepromSize);
    uint16_t packetSize;
    bool resultBool = SavePacketToEEPROM(0, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == true);
}

void Helper_SavePacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_WeProvideNoEepromSize_SetsEepromSizeAutomaticallyToFitOurPacket()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);

    Helper_SavePacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, dataPtr, sizeof(data));
    uint16_t packetSize;
    bool resultBool = SavePacketToEEPROM(0, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == true);
}

void Helper_SavePacketToEEPROM_PreparesEepromSizeAndPrepareToReturnPacket_UsingHighAdress_EepromLengthIsSetToFitPacket()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    uint16_t adress = 10000;

    Helper_SavePacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, dataPtr, sizeof(data));
    uint16_t packetSize;
    bool resultBool = SavePacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

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
