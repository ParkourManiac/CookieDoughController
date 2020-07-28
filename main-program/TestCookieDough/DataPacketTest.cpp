#include "test.h"
#include "../lib/CookieDoughLibrary/DataPacket.h"
#include "Fakes/Arduino.h"
#include "Fakes/EEPROM.h"

extern uint8_t EEPROMClass_read_return;
extern std::vector<uint8_t> EEPROMClass_read_return_v;
extern unsigned int EEPROMClass_read_invocations;
extern int EEPROMClass_read_param_idx;
extern std::vector<int> EEPROMClass_read_param_idx_v;

extern uint8_t EEPROMClass_put_return_o1;
extern std::vector<uint8_t> EEPROMClass_put_return_o1_v;
extern unsigned int EEPROMClass_put_invocations_o1;
extern int EEPROMClass_put_param_idx_o1;
extern std::vector<int> EEPROMClass_put_param_idx_o1_v;
extern uint8_t EEPROMClass_put_param_t_o1;
extern std::vector<uint8_t> EEPROMClass_put_param_t_o1_v;

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

void TestIfVectorTestsAreWorking_ShouldReturnDifferentValuesEachTime()
{
    EEPROMClass_read_return_v.push_back(1);
    EEPROMClass_read_return_v.push_back(2);
    EEPROMClass_read_return_v.push_back(3);
    EEPROMClass_read_return_v.push_back(4);

    uint8_t first = EEPROM.read(0);
    uint8_t second = EEPROM.read(0);
    uint8_t third = EEPROM.read(0);
    uint8_t fourth = EEPROM.read(0);

    ASSERT_TEST(first == 1 && second == 2 && third == 3 && fourth == 4);
}

void TestIfVectorTestsAreWorking_ShouldReturnDifferentParametersEachTime()
{
    for (int i = 0; i < 4; i++)
        EEPROMClass_read_return_v.push_back(0);

    EEPROM.read(1);
    EEPROM.read(2);
    EEPROM.read(3);
    EEPROM.read(4);
    uint8_t first = EEPROMClass_read_param_idx_v[0];
    uint8_t second = EEPROMClass_read_param_idx_v[1];
    uint8_t third = EEPROMClass_read_param_idx_v[2];
    uint8_t fourth = EEPROMClass_read_param_idx_v[3];

    ASSERT_TEST(first == 1 && second == 2 && third == 3 && fourth == 4);
}

// TODO: WRITE A TEST FOR DATAPACKET WHERE YOU NEED TO USE THE SAME MOCKED FUNCITON MULTIPLE TIMES.






void SavePacketToEEPROM_SavesStxToFirstGivenAdress()
{
    uint8_t data = 42;
    unsigned int adress = 20;
    unsigned int packetSize;

    printf("\n");
    printf("%d", adress);
    printf("\n");
    SavePacketToEEPROM(adress, &data, sizeof(data), packetSize);
    printf("%d", EEPROMClass_put_param_idx_o1_v[0]);
    printf("\n");

    ASSERT_TEST(EEPROMClass_put_param_idx_o1_v[0] == adress);
}