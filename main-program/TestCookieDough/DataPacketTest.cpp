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


void DataPacket_Constructor_NoArguments_AllocatesSpaceForPayload()
{
    DataPacket packet;

    bool result = packet.payload;

    ASSERT_TEST(result == true);
}

void DataPacket_Constructor_WithArguments_CopiesDataIntoPayload()
{
    uint16_t data = 58;
    uint16_t dataSize = sizeof(data);
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);

    DataPacket packet = DataPacket(dataPtr, dataSize);

    bool packetContainsCorrectData = (
        dataPtr[0] == packet.payload[0] &&
        dataPtr[1] == packet.payload[1]
    );
    data = 33;
    dataSize = sizeof(data);
    dataPtr = reinterpret_cast<uint8_t*>(&data);
    bool packetDataIsNotConnectedToProvidedData = (
        dataPtr[0] != packet.payload[0] ||
        dataPtr[1] != packet.payload[1]
    );
    ASSERT_TEST(packetContainsCorrectData && packetDataIsNotConnectedToProvidedData);
}

void DataPacket_Constructor_WithArguments_SetsUpPacketCorrectly()
{
    uint16_t data = 58;
    uint16_t dataSize = sizeof(data);
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);

    DataPacket packet = DataPacket(dataPtr, dataSize);

    bool packetContainsCorrectData = (
        dataPtr[0] == packet.payload[0] &&
        dataPtr[1] == packet.payload[1]
    );
    ASSERT_TEST(
        packetContainsCorrectData &&
        IsPacketActive(packet.active) &&
        packet.crc == CalculateCRC(dataPtr, dataSize) &&
        packet.payloadLength == dataSize
    );
}

void DataPacket_CopyConstructor_CopiesValuesAndPayload()
{
    uint16_t data = 15666;
    DataPacket other = DataPacket(reinterpret_cast<uint8_t*>(&data), sizeof(data));
    other.active = 0x13;

    DataPacket result = DataPacket(other);

    ASSERT_TEST(
        result.stx == other.stx &&
        result.active == other.active &&
        result.payloadLength == other.payloadLength &&
        result.crc == other.crc &&
        result.payload[0] == other.payload[0] &&
        result.payload[1] == other.payload[1] &&
        result.etx == other.etx
    );
}

void DataPacket_CopyConstructor_PayloadIsCopiedAndIndependent()
{
    uint16_t data = 15666;
    DataPacket other = DataPacket(reinterpret_cast<uint8_t*>(&data), sizeof(data));
    DataPacket expected = DataPacket(reinterpret_cast<uint8_t*>(&data), sizeof(data));
    other.active = 0x13;
    expected.active = 0x13;

    DataPacket result = DataPacket(other);
    other.active = 0x00;
    other.payloadLength = 0;
    other.payload[0] = 0;
    other.payload[1] = 0;
    other.crc = 0;

    ASSERT_TEST(
        result.stx == expected.stx &&
        result.active == expected.active &&
        result.payloadLength == expected.payloadLength &&
        result.crc == expected.crc &&
        result.payload[0] == expected.payload[0] &&
        result.payload[1] == expected.payload[1] &&
        result.etx == expected.etx
    );
}

void DataToPacket_TakesInDataOfTypeT_ConvertsItCorrectlyIntoADataPacket()
{
    uint16_t data = 65530;
    uint8_t *expectedDataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket expectedPacket = DataPacket(expectedDataPtr, sizeof(data));

    DataPacket result = DataToPacket(data);

    ASSERT_TEST(
        result.stx == expectedPacket.stx &&
        result.active == expectedPacket.active &&
        result.payloadLength == expectedPacket.payloadLength &&
        result.crc == expectedPacket.crc &&
        result.payload[0] == expectedPacket.payload[0] &&
        result.payload[1] == expectedPacket.payload[1] &&
        result.etx == expectedPacket.etx
    );
}

void DataToPacket_TakesInDataOfTypeBool_CanHandleDifferentTypes()
{
    bool data = false;
    uint8_t *expectedDataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket expectedPacket = DataPacket(expectedDataPtr, sizeof(data));

    DataPacket result = DataToPacket(data);

    ASSERT_TEST(
        result.stx == expectedPacket.stx &&
        result.active == expectedPacket.active &&
        result.payloadLength == expectedPacket.payloadLength &&
        result.crc == expectedPacket.crc &&
        result.payload[0] == expectedPacket.payload[0] &&
        result.payload[1] == expectedPacket.payload[1] &&
        result.etx == expectedPacket.etx
    );
}

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
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    unsigned int expectedStxAdress = static_cast<int>(adress), 
                 expectedActiveFlagAdress = expectedStxAdress + sizeof(packet.stx), 
                 expectedPayloadLengthAdress = expectedActiveFlagAdress + sizeof(packet.active), 
                 expectedCRCAdress = expectedPayloadLengthAdress + sizeof(packet.payloadLength), 
                 expectedPayloadAdress = expectedCRCAdress + sizeof(packet.crc), 
                 expectedEtxAdress = expectedPayloadAdress + sizeof(data), 
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

void ParsePacketFromEEPROM_DataPacketDoesNotNeedToBeManuallyAllocatedBeforePassedToFunciton_DoesNotCrash()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

    DataPacket result;
    uint16_t packetSize;
    bool didNotCrash = false;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);
    didNotCrash = true;

    ASSERT_TEST(
        resultBool == true &&
        result.payload[0] == packet.payload[0] &&
        result.payload[1] == packet.payload[1] &&
        didNotCrash == true
    );
}

void ParsePacketFromEEPROM_ReturnsCorrectPacket() // TODO: bad test. Locks the order the mocked functions will be called. Rewrite with different framework?
{
    uint16_t adress = 13;
    uint16_t data = 421;
    DataPacket expectedPacket = DataPacket(reinterpret_cast<uint8_t*>(&data), sizeof(data)); // packet.active is active by default.
    uint16_t expectedPacketSize = sizeof(expectedPacket.stx) + 
                                  sizeof(expectedPacket.active) + 
                                  sizeof(expectedPacket.payloadLength) + 
                                  sizeof(expectedPacket.crc) + 
                                  sizeof(data) + 
                                  sizeof(expectedPacket.etx);

    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o3_vr.push_back(expectedPacket.active);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_length_return = static_cast<uint16_t>(expectedPacket.payloadLength + 20);
    EEPROMClass_get_param_t_o2_vr.push_back(expectedPacket.crc);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);
    EEPROMClass_read_return_v.push_back(expectedPacket.payload[0]);
    EEPROMClass_read_return_v.push_back(expectedPacket.payload[1]);

    DataPacket result;
    uint16_t packetSizeResult;
    bool resultBool = ParsePacketFromEEPROM(adress, &result, &packetSizeResult);

    ASSERT_TEST(
        resultBool == true &&
        expectedPacket.stx == result.stx &&
        IsPacketActive(result.active) &&
        expectedPacket.active == result.active && 
        expectedPacket.payloadLength == result.payloadLength &&
        expectedPacket.crc == result.crc &&
        expectedPacket.payload[0] == result.payload[0] &&
        expectedPacket.payload[1] == result.payload[1] &&
        expectedPacket.etx == result.etx &&
        expectedPacketSize == packetSizeResult
    );
}

void ParsePacketFromEEPROM_ReturnsFalseWhenAValidPacketIsNotActive()
{
    uint64_t data = 123211321;
    DataPacket packet = DataPacket(reinterpret_cast<uint8_t *>(&data), sizeof(data));
    packet.active = 0x00;

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
}

void ParsePacketFromEEPROM_ReturnsTrueWhenAValidPacketIsActive()
{
    uint64_t data = 123211321;
    DataPacket packet = DataPacket(reinterpret_cast<uint8_t *>(&data), sizeof(data)); // packet.active is set to be active by default.

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == true);
}

void ParsePacketFromEEPROM_EepromReturnsFaultyData_ReturnsFalse() // TODO: bad test. Locks the order the mocked functions will be called. Rewrite with different framework?
{
    uint16_t adress = 13;
    uint16_t data = 421;
    DataPacket expectedPacket = DataPacket(reinterpret_cast<uint8_t*>(&data), sizeof(data));

    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_length_return = static_cast<uint16_t>(expectedPacket.payloadLength + 20);
    EEPROMClass_get_param_t_o2_vr.push_back(expectedPacket.crc);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);
    EEPROMClass_read_return_v.push_back(static_cast<uint8_t>(expectedPacket.payload[0] + 13));
    EEPROMClass_read_return_v.push_back(expectedPacket.payload[1]);
 
    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ParsePacketFromEEPROM(adress, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
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





// TODO: Fullfill these tests

// void DeactivatePacket_OverritesCorrectByteWithADeactivatedFlag() // Todo: Fake valid packet on eeprom. Use info below...
// {
//     uint16_t adress = 23;
//     uint16_t deactivatedFlag = 0x00;
//     DataPacket packet;
//     uint16_t expectedOverwrittenAdress = adress + sizeof(packet.stx);

//     DeactivatePacket(adress);

//     ASSERT_TEST(
//         EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress &&
//         EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag
//     );
// }

// void DeactivatePacket_DeactivatesPacketSuccessfully_ReturnsTrue() // Todo: Fake valid packet on eeprom. Use info below...
// {
//     uint16_t adress = 23;
//     uint16_t deactivatedFlag = 0x00;

//     bool result = DeactivatePacket(adress);

//     ASSERT_TEST(
//         result == true &&
//         EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress &&
//         EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag
//     );
// }

// void DeactivatePacket_AdressDoesNotPointToStx_ReturnsFalse()
// {
//     uint16_t adress = 23;
//     uint16_t deactivatedFlag = 0x00;
//     EEPROMClass_read_return_v.push_back(0x00);

//     bool result = DeactivatePacket(adress);

//     ASSERT_TEST(result == false);
// }

//void DeactivatePacket_AdressDoesNotPointToStx_DoesNotWriteToEEPROM() // Invocations == 0


// TODO:
// void DeactivatePacket_AdressPointsToStxButCantFindEtxOfPacket_ReturnsFalse();
// void DeactivatePacket_AdressPointsToStxButCantFindEtxOfPacket_DoesNotWriteToEEPROM(); // Invocations == 0


/*
// TODO: TO make DeactivatePacket succeed, do this:
    DataPacket expectedPacket;
    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_read_return_v.push_back(exptedPacket.etx);
*/