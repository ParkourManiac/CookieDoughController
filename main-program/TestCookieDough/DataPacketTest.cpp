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