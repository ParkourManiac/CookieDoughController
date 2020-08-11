#include "../test.h"
#include "TestHelper/DataPacketHelper.h"

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket_ParsePacketFromEepromSuccessfullyReturnsCorrectPacket() {
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
    bool resultBool = ParsePacketFromEEPROM(0, result, packetSize);
    printf("%d", packetSize);
    printf("\n");
    printf("%d", packet.payload[1]);

    ASSERT_TEST(resultBool == true &&
                packet.stx == result.stx &&
                packet.payloadLength == result.payloadLength &&
                packet.crc == result.crc &&
                *((uint16_t*)packet.payload) == data &&
                packet.etx == result.etx &&
                packetSize == 10);
    delete(resultPtr);
}