#include "test.h"
#include "../lib/CookieDoughLibrary/DataPacket.h"
#include "Fakes/Arduino.h"

void DataPacket_StxIsTwo()
{
    DataPacket packet;

    ASSERT_TEST(packet.stx == 0x02);
}

void DataPacket_EtxIsThree()
{
    DataPacket packet;

    ASSERT_TEST(packet.etx == 0x03);
}

void CalculateCRC_UsesAlgorithCRC32()
{
    uint8_t data = 0xF;
    uint8_t *dataPtr = &data;

    unsigned long result = CalculateCRC(dataPtr, sizeof(data));
    ASSERT_TEST(result == 1119744540);
}