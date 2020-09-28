#include "../test.h"
#include "TestHelper/DataPacketHelper.h"

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_ParsePacketFromEepromSuccessfullyReturnsCorrectPacket() {
    uint16_t data = 1337;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);
    DataPacket result;
    result.payload = new uint8_t[1];
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == true &&
                packet.stx == result.stx &&
                packet.active == result.active &&
                packet.payloadLength == result.payloadLength &&
                packet.crc == result.crc &&
                *(reinterpret_cast<uint16_t*>(packet.payload)) == data && // TODO: Is this the correct way to retrieve the data?
                packet.etx == result.etx &&
                packetSize == Helper_CalculateSizeOfPacketOnEEPROM(packet));
    delete[](result.payload);
}