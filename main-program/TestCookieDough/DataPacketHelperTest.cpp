#include "../test.h"
#include "TestHelper/DataPacketHelper.h"

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_ParsePacketFromEepromSuccessfullyReturnsCorrectPacket() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);
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

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_SetEepromByHandToFitThePacket_ParsePacketFromEepromSuccessfullyReturnsCorrectPacket() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet, static_cast<uint16_t>(65535u));
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

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_SetEepromByHandToNotFitThePacket_ParsePacketFromEEPROMFails() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet, 1);
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

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
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