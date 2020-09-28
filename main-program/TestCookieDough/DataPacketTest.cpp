#include "test.h"
#include "../lib/CookieDoughLibrary/DataPacket.h"
#include "Fakes/Arduino.h"
#include "Fakes/EEPROM.h"
#include "TestHelper/DataPacketHelper.h"

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

extern uint16_t EEPROMClass_get_return_o1;
extern std::vector<uint16_t> EEPROMClass_get_return_o1_v;
extern unsigned int EEPROMClass_get_invocations_o1;
extern int EEPROMClass_get_param_idx_o1;
extern std::vector<int> EEPROMClass_get_param_idx_o1_v;
extern uint16_t EEPROMClass_get_param_t_o1;
extern std::vector<uint16_t> EEPROMClass_get_param_t_o1_v;
extern uint16_t EEPROMClass_get_param_t_o1_r;
extern std::vector<uint16_t> EEPROMClass_get_param_t_o1_vr;

extern uint32_t EEPROMClass_get_return_o2;
extern std::vector<uint32_t> EEPROMClass_get_return_o2_v;
extern unsigned int EEPROMClass_get_invocations_o2;
extern int EEPROMClass_get_param_idx_o2;
extern std::vector<int> EEPROMClass_get_param_idx_o2_v;
extern uint32_t EEPROMClass_get_param_t_o2;
extern std::vector<uint32_t> EEPROMClass_get_param_t_o2_v;
extern uint32_t  EEPROMClass_get_param_t_o2_r;
extern std::vector<uint32_t > EEPROMClass_get_param_t_o2_vr;

extern std::vector<uint8_t > EEPROMClass_get_param_t_o3_vr;

extern uint16_t EEPROMClass_length_return;
extern unsigned int EEPROMClass_length_invocations;

void DataPacket_ByDefault_StxIsTwo()
{
    DataPacket packet;

    ASSERT_TEST(packet.stx == 0x02);
}

void DataPacket_ByDefault_EtxIsThree()
{
    DataPacket packet;

    ASSERT_TEST(packet.etx == 0x03);
}

void DataPacket_ByDefault_ActiveIsTrue()
{
    DataPacket packet;

    bool result = IsPacketActive(packet.active);

    ASSERT_TEST(result == true);
}

void CalculateCRC_UsesAlgorithCRC32()
{
    uint8_t data = 0xF;
    uint8_t *dataPtr = &data;

    uint32_t result = CalculateCRC(dataPtr, sizeof(data));
    ASSERT_TEST(result == 1119744540);
}

void SavePacketToEEPROM_SavesStxToFirstGivenAdress()
{
    uint8_t data = 42;
    uint16_t adress = 20;
    uint16_t packetSize;

    SavePacketToEEPROM(adress, &data, sizeof(data), &packetSize);

    ASSERT_TEST(EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(adress));
}

void SavePacketToEEPROM_EtxIsPutDownAtTheEndOfThePacket()
{
    DataPacket packet;
    uint8_t data = 42;
    uint16_t adress = 20;
    uint16_t packetSize;

    SavePacketToEEPROM(adress, &data, sizeof(data), &packetSize);
    unsigned int expectedEtxPosition = adress + sizeof(packet.stx) + sizeof(packet.active) + sizeof(packet.payloadLength) + sizeof(packet.crc) + sizeof(data);

    ASSERT_TEST(EEPROMClass_put_param_idx_o1_v[2] == static_cast<int>(expectedEtxPosition) && EEPROMClass_put_param_t_o1_v[2] == packet.etx);
}

void SavePacketToEEPROM_PacketIsCorrectlyPutDown()
{
    uint16_t data = 42;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    uint16_t adress = 20;
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);
    unsigned int expectedStxAdress, expectedActiveFlagAdress, expectedPayloadLengthAdress, expectedCRCAdress, expectedPayloadAdress, expectedEtxAdress, expectedPacketSize;
    expectedStxAdress = static_cast<int>(adress);
    expectedActiveFlagAdress = expectedStxAdress + sizeof(packet.stx);
    expectedPayloadLengthAdress = expectedActiveFlagAdress + sizeof(packet.active);
    expectedCRCAdress = expectedPayloadLengthAdress + sizeof(packet.payloadLength);
    expectedPayloadAdress = expectedCRCAdress + sizeof(packet.crc);
    expectedEtxAdress = expectedPayloadAdress + sizeof(data);
    expectedPacketSize = sizeof(packet.stx) + sizeof(packet.active) + sizeof(packet.payloadLength) + sizeof(packet.crc) + sizeof(data) + sizeof(packet.etx);
    // This ensures that ParsePacketFromEEPROM returns true
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

    uint16_t packetSize;
    bool resultBool = SavePacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == true && 
                EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(expectedStxAdress) && EEPROMClass_put_param_t_o1_v[0] == packet.stx &&
                EEPROMClass_put_param_idx_o1_v[1] == static_cast<int>(expectedActiveFlagAdress) && EEPROMClass_put_param_t_o1_v[1] == packet.active &&
                EEPROMClass_put_param_idx_o2_v[0] == static_cast<int>(expectedPayloadLengthAdress) && EEPROMClass_put_param_t_o2_v[0] == sizeof(data) &&
                EEPROMClass_put_param_idx_o3_v[0] == static_cast<int>(expectedCRCAdress) && EEPROMClass_put_param_t_o3_v[0] == CalculateCRC(dataPtr, sizeof(data)) &&
                EEPROMClass_update_param_idx_v[0] == static_cast<int>(expectedPayloadAdress) && EEPROMClass_update_param_val_v[0] == dataPtr[0] &&
                EEPROMClass_update_param_idx_v[1] == static_cast<int>(expectedPayloadAdress) + 1 && EEPROMClass_update_param_val_v[1] == dataPtr[1] &&
                EEPROMClass_put_param_idx_o1_v[2] == static_cast<int>(expectedEtxAdress) && EEPROMClass_put_param_t_o1_v[2] == packet.etx &&
                packetSize == expectedPacketSize);
}

void SavePacketToEEPROM_PacketIsSavedButEepromFailsToReadTheData_ReturnsFalse()
{
    uint16_t data = 42;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    uint16_t adress = 20;
    DataPacket packet;

    // This ensures that ParsePacketFromEEPROM returns false
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_read_return_v.push_back(packet.active);
    EEPROMClass_get_param_t_o1_vr.push_back(sizeof(data));
    EEPROMClass_length_return = sizeof(data) + 20;
    EEPROMClass_get_param_t_o2_vr.push_back(55561893);
    EEPROMClass_read_return_v.push_back(packet.etx);
    EEPROMClass_read_return_v.push_back(static_cast<uint8_t>(dataPtr[0] + 13));
    EEPROMClass_read_return_v.push_back(dataPtr[1]);

    uint16_t packetSize;
    bool resultBool = SavePacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == false);
}

void SavePacketToEEPROM_AdaptsSizeOfPacketToFitData()
{
    DataPacket packet;
    uint16_t data = 42;
    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(&data);
    uint16_t adress = 20;
    uint16_t packetSize;

    int expectedPayloadAdress = static_cast<int>(adress + sizeof(packet.stx) + sizeof(packet.active) + sizeof(packet.payloadLength) + sizeof(packet.crc));

    SavePacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(EEPROMClass_update_param_idx_v[0] == expectedPayloadAdress && EEPROMClass_update_param_val_v[0] == dataPtr[0] &&
                EEPROMClass_update_param_idx_v[1] == expectedPayloadAdress + 1 && EEPROMClass_update_param_val_v[1] == dataPtr[1]);
}

void ParsePacketFromEEPROM_ReturnsCorrectPacket() // TODO: bad test. Locks the order the mocked functions will be called. Rewrite with different framework?
{
    uint16_t adress = 13;
    uint16_t data = 421;
    DataPacket expectedPacket;
    expectedPacket.payloadLength = sizeof(data);
    expectedPacket.payload = reinterpret_cast<uint8_t*>(&data);
    expectedPacket.crc = CalculateCRC(expectedPacket.payload, expectedPacket.payloadLength);
    uint16_t expectedPacketSize = sizeof(expectedPacket.stx) + sizeof(expectedPacket.active) + sizeof(expectedPacket.payloadLength) + 
                                    sizeof(expectedPacket.crc) + sizeof(data) + sizeof(expectedPacket.etx);

    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o3_vr.push_back(expectedPacket.active);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_length_return = static_cast<uint16_t>(expectedPacket.payloadLength + 20);
    EEPROMClass_get_param_t_o2_vr.push_back(expectedPacket.crc);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);
    EEPROMClass_read_return_v.push_back(expectedPacket.payload[0]);
    EEPROMClass_read_return_v.push_back(expectedPacket.payload[1]);

    DataPacket result;
    result.payload = new uint8_t[1];
    uint16_t packetSizeResult;
    bool resultBool = ParsePacketFromEEPROM(adress, &result, &packetSizeResult);

    ASSERT_TEST(resultBool == true &&
                expectedPacket.stx == result.stx &&
                expectedPacket.active == result.active && 
                expectedPacket.payloadLength == result.payloadLength &&
                expectedPacket.crc == result.crc &&
                expectedPacket.payload[0] == result.payload[0] &&
                expectedPacket.payload[1] == result.payload[1] &&
                expectedPacket.etx == result.etx &&
                expectedPacketSize == packetSizeResult);
    delete[](result.payload);
}

void ParsePacketFromEEPROM_EepromReturnsFaultyData_ReturnsFalse() // TODO: bad test. Locks the order the mocked functions will be called. Rewrite with different framework?
{
    uint16_t adress = 13;
    uint16_t data = 421;
    DataPacket expectedPacket;
    expectedPacket.payloadLength = sizeof(data);
    expectedPacket.payload = reinterpret_cast<uint8_t*>(&data);
    expectedPacket.crc = CalculateCRC(expectedPacket.payload, expectedPacket.payloadLength);

    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_length_return = static_cast<uint16_t>(expectedPacket.payloadLength + 20);
    EEPROMClass_get_param_t_o2_vr.push_back(expectedPacket.crc);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);
    EEPROMClass_read_return_v.push_back(static_cast<uint8_t>(expectedPacket.payload[0] + 13));
    EEPROMClass_read_return_v.push_back(expectedPacket.payload[1]);
 
    DataPacket result;
    result.payload = new uint8_t[1];
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(adress, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
    delete[](result.payload);
}

void IsPacketActive_PacketsActiveFlagIsOne_ReturnsTrue() 
{
    DataPacket packet;
    packet.active = 0x01;

    bool result = IsPacketActive(packet.active);
    
    ASSERT_TEST(result == true);
}

void IsPacketActive_PacketsActiveFlagIsZero_ReturnsFalse() 
{
    DataPacket packet;
    packet.active = 0x00;

    bool result = IsPacketActive(packet.active);
    
    ASSERT_TEST(result == false);
}

void IsPacketActive_PacketsActiveFlagIsNotOne_ReturnsFalse() 
{
    DataPacket packet;
    packet.active = 0x84;

    bool result = IsPacketActive(packet.active);
    
    ASSERT_TEST(result == false);
}