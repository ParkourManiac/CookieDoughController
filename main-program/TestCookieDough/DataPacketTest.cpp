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

extern uint16_t EEPROMClass_put_return_o2;
extern std::vector<uint16_t> EEPROMClass_put_return_o2_v;
extern unsigned int EEPROMClass_put_invocations_o2;
extern int EEPROMClass_put_param_idx_o2;
extern std::vector<int> EEPROMClass_put_param_idx_o2_v;
extern uint16_t EEPROMClass_put_param_t_o2;
extern std::vector<uint16_t> EEPROMClass_put_param_t_o2_v;

extern uint32_t EEPROMClass_put_return_o3;
extern std::vector<uint32_t> EEPROMClass_put_return_o3_v;
extern unsigned int EEPROMClass_put_invocations_o3;
extern int EEPROMClass_put_param_idx_o3;
extern std::vector<int> EEPROMClass_put_param_idx_o3_v;
extern uint32_t EEPROMClass_put_param_t_o3;
extern std::vector<uint32_t> EEPROMClass_put_param_t_o3_v;

extern unsigned int EEPROMClass_update_invocations;
extern int EEPROMClass_update_param_idx;
extern std::vector<int> EEPROMClass_update_param_idx_v;
extern uint8_t EEPROMClass_update_param_val;
extern std::vector<uint8_t> EEPROMClass_update_param_val_v;

extern uint16_t  EEPROMClass_get_return_o1;
extern std::vector<uint16_t > EEPROMClass_get_return_o1_v;
extern unsigned int EEPROMClass_get_invocations_o1;
extern int EEPROMClass_get_param_idx_o1;
extern std::vector<int> EEPROMClass_get_param_idx_o1_v;
extern uint16_t  EEPROMClass_get_param_t_o1;
extern std::vector<uint16_t > EEPROMClass_get_param_t_o1_v;

extern uint32_t  EEPROMClass_get_return_o2;
extern std::vector<uint32_t > EEPROMClass_get_return_o2_v;
extern unsigned int EEPROMClass_get_invocations_o2;
extern int EEPROMClass_get_param_idx_o2;
extern std::vector<int> EEPROMClass_get_param_idx_o2_v;
extern uint32_t  EEPROMClass_get_param_t_o2;
extern std::vector<uint32_t > EEPROMClass_get_param_t_o2_v;

extern uint16_t EEPROMClass_length_return;
extern unsigned int EEPROMClass_length_invocations;

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

    SavePacketToEEPROM(adress, &data, sizeof(data), packetSize);

    ASSERT_TEST(EEPROMClass_put_param_idx_o1_v[0] == adress);
}

void SavePacketToEEPROM_EtxIsPutDownAtTheEndOfThePacket()
{
    DataPacket packet;
    uint8_t data = 42;
    unsigned int adress = 20;
    unsigned int packetSize;

    SavePacketToEEPROM(adress, &data, sizeof(data), packetSize);
    unsigned int expectedEtxPosition = adress + sizeof(packet.etx) + sizeof(packet.payloadLength) + sizeof(packet.crc) + sizeof(data);

    ASSERT_TEST(EEPROMClass_put_param_idx_o1_v[1] == expectedEtxPosition && EEPROMClass_put_param_t_o1_v[1] == packet.etx);
}

void SavePacketToEEPROM_PacketIsCorrectlyPutDown()
{
    uint8_t data = 42;
    unsigned int adress = 20;
    DataPacket packet;
    unsigned int expectedStxAdress = adress;
    unsigned int expectedPayloadLengthAdress = adress + sizeof(packet.stx);
    unsigned int expectedCRCAdress = adress + sizeof(packet.stx) + sizeof(packet.payloadLength);
    unsigned int expectedPayloadAdress = adress + sizeof(packet.stx) + sizeof(packet.payloadLength) + sizeof(packet.crc);
    unsigned int expectedEtxAdress = adress + sizeof(packet.stx) + sizeof(packet.payloadLength) + sizeof(packet.crc) + sizeof(data);

    unsigned int packetSize;
    adress + sizeof(packet.etx) + sizeof(packet.payloadLength) + sizeof(packet.crc) + sizeof(data) + sizeof(packet.etx);

    SavePacketToEEPROM(adress, &data, sizeof(data), packetSize);

    ASSERT_TEST(EEPROMClass_put_param_idx_o1_v[0] == adress && EEPROMClass_put_param_t_o1_v[0] == packet.stx &&
                EEPROMClass_put_param_idx_o2_v[0] == expectedPayloadLengthAdress && EEPROMClass_put_param_t_o2_v[0] == sizeof(data) &&
                EEPROMClass_put_param_idx_o3_v[0] == expectedCRCAdress && EEPROMClass_put_param_t_o3_v[0] == CalculateCRC(&data, sizeof(data)) &&
                EEPROMClass_update_param_idx_v[0] == expectedPayloadAdress && EEPROMClass_update_param_val_v[0] == (&data)[0] &&
                EEPROMClass_put_param_idx_o1_v[1] == expectedEtxAdress && EEPROMClass_put_param_t_o1_v[1] == packet.etx);
}

void SavePacketToEEPROM_AdaptsSizeOfPacketToFitData()
{
    DataPacket packet;
    uint16_t data = 42;
    unsigned int adress = 20;
    unsigned int packetSize;

    unsigned int expectedPayloadAdress = adress + sizeof(packet.stx) + sizeof(packet.payloadLength) + sizeof(packet.crc);

    SavePacketToEEPROM(adress, (uint8_t *)&data, sizeof(data), packetSize);

    ASSERT_TEST(EEPROMClass_update_param_idx_v[0] == expectedPayloadAdress && EEPROMClass_update_param_val_v[0] == ((uint8_t *)&data)[0] &&
                EEPROMClass_update_param_idx_v[1] == expectedPayloadAdress + 1 && EEPROMClass_update_param_val_v[1] == ((uint8_t *)&data)[1]);
}

// void ParsePacketFromEEPROM_ReturnsCorrectPackage_Part1()
// {
//     unsigned int adress = 13;
//     uint16_t data = 42;
//     DataPacket expectedPacket;
//     expectedPacket.payloadLength = sizeof(data);
//     expectedPacket.payload = (uint8_t*) &data;
//     expectedPacket.crc = 1337;

//     EEPROMClass_read_return_v.push_back(expectedPacket.stx);
//     EEPROMClass_get_return_o1_v.push_back(expectedPacket.payloadLength);
//     EEPROMClass_get_return_o2_v.push_back(expectedPacket.crc);
//     EEPROMClass_read_return_v.push_back(expectedPacket.etx);
//     EEPROMClass_read_return_v.push_back(expectedPacket.payload[0]);
//     EEPROMClass_read_return_v.push_back(expectedPacket.payload[1]);
//     EEPROMClass_length_return = expectedPacket.payloadLength + 20;

//     DataPacket result;
//     result.payloadLength = expectedPacket.payloadLength;
//     unsigned int packetSize;
//     bool resultBool = ParsePacketFromEEPROM(adress, result, packetSize);
//     printf("%d", EEPROMClass_get_invocations_o2);


//     // uint8_t stx = EEPROM.read(currentAdress);
//     // EEPROM.get(currentAdress, packet.payloadLength);
//     // EEPROM.get(currentAdress, packet.crc);
//     // EEPROM.read(currentAdress + packet.payloadLength) != packet.etx
//     // payload[i] = EEPROM.read(currentAdress + i);

//     ASSERT_TEST(expectedPacket.etx == result.etx &&
//                 expectedPacket.payloadLength == result.payloadLength &&
//                 expectedPacket.crc == result.crc &&
//                 expectedPacket.payload[0] == result.payload[0] &&
//                 expectedPacket.payload[1] == result.payload[1] &&
//                 expectedPacket.etx == result.etx);
// }

// TODO: Mock framework can't mock references.