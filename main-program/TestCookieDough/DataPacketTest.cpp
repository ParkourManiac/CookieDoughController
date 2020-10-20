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
extern std::vector<int> EEPROMClass_get_param_idx_o3_v;

extern uint16_t EEPROMClass_length_return;
extern std::vector<uint16_t> EEPROMClass_length_return_v;
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

void CalculateCRC_DataCanBeMadeStreamable()
{
    uint32_t data = 98547324;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    uint8_t data0 = dataPtr[0],
            data1 = dataPtr[1],
            data2 = dataPtr[2],
            data3 = dataPtr[3];
    uint32_t expectedResult = CalculateCRC(dataPtr, sizeof(data));

    uint32_t result = CalculateCRC(&data0, sizeof(data0));
    result = CalculateCRC(&data1, sizeof(data1), result);
    result = CalculateCRC(&data2, sizeof(data2), result);
    result = CalculateCRC(&data3, sizeof(data3), result);

    ASSERT_TEST(result == expectedResult);
}

void CyclicAdress_TakesInAnAdressThatExceedsTheBufferSize_WrapsBackToTheBeginningOfTheBuffer()
{
    uint16_t bufferSize = 1024;
    uint16_t adress = static_cast<uint16_t>(bufferSize + 13);
    uint16_t expectedAdress = static_cast<uint16_t>(adress % bufferSize);

    uint16_t result = CyclicAdress(adress, bufferSize);

    ASSERT_TEST(result == expectedAdress);
}

void CyclicAdress_AdressOvershootsLastBufferAdressWithOneStep_ReturnsTheFirstAdressOftheBuffer()
{
    uint16_t bufferSize = 1024;
    uint16_t adress = static_cast<uint16_t>(1023 + 1);
    uint16_t expectedAdress = 0;

    uint16_t result = CyclicAdress(adress, bufferSize);

    ASSERT_TEST(result == expectedAdress);
}

void CyclicAdress_TakesInAnAdressThatExceedsTheLastAdressOfTheBuffer_CalculatesTheNewAdressCorrectly()
{
    uint16_t bufferSize = 1024;
    uint16_t overshoot = 25;
    uint16_t adress = static_cast<uint16_t>(1023 + overshoot);
    uint16_t expectedAdress = static_cast<uint16_t>(overshoot - 1);

    uint16_t result = CyclicAdress(adress, bufferSize);

    ASSERT_TEST(result == expectedAdress);
}

void CyclicAdress_OvershootsWholeBufferMultipleTimes_ReturnsAdressWithinBuffersSize()
{
    uint16_t bufferSize = 10;
    uint16_t adress = 35;
    uint16_t expectedAdress = 5;

    uint16_t result = CyclicAdress(adress, bufferSize);

    ASSERT_TEST(result == expectedAdress);
}

void CyclicAdress_CalculatesNewAdressDependingOnBufferSize_ReturnsCorrectAdress()
{
    uint16_t bufferSize1 = 10;
    uint16_t bufferSize2 = 3;
    uint16_t adress = 35;
    uint16_t expectedAdress1 = 5;
    uint16_t expectedAdress2 = 2;

    uint16_t result1 = CyclicAdress(adress, bufferSize1);
    uint16_t result2 = CyclicAdress(adress, bufferSize2);

    ASSERT_TEST(
        result1 == expectedAdress1 &&
        result2 == expectedAdress2
    );
}

void CyclicAdress_ProvidedAdressIsWithinBuffersSize_AdressIsUnchanged()
{
    uint16_t bufferSize = 1024;
    uint16_t expectedAdress = 103;

    uint16_t result = CyclicAdress(expectedAdress, bufferSize);

    ASSERT_TEST(result == expectedAdress);
}

void CyclicAdress_BufferSizeIsZero_ReturnsZero() 
{
    uint16_t bufferSize = 0,
            adress = 13,
            expectedAdress = 0;

    uint16_t result = CyclicAdress(adress, bufferSize);

    ASSERT_TEST(result == expectedAdress);
}

void SizeOfSerializedDataPacket_ReturnsTheAmountOfBytesTheDataPacketWillOccupyOnMemory()
{
    uint64_t data = 981239;
    DataPacket packet = DataToPacket(data);
    uint16_t expectedSize = static_cast<uint16_t>(
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc) +
        sizeof(data) +
        sizeof(packet.etx)
    );

    uint16_t result = SizeOfSerializedDataPacket(packet);

    ASSERT_TEST(result == expectedSize);
}

void SizeOfSerializedDataPacket_RecievesPacketWithoutAPayload_StillCalculatesSizeOfRemainingPacket()
{
    uint8_t data = 0,
            dataSize = 0;
    DataPacket packet = DataPacket(&data, dataSize);
    uint16_t expectedSize = static_cast<uint16_t>(
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc) +
        sizeof(packet.etx)
    );

    uint16_t result = SizeOfSerializedDataPacket(packet);

    ASSERT_TEST(result == expectedSize);
}

void SizeOfSerializedDataPacket_CalculatesSizeDependingOnTheSizeOfThePayload()
{
    uint64_t bigData = 981239;
    uint8_t smallData = 123;
    DataPacket packetTemplate = DataPacket(),
               bigPacket = DataToPacket(bigData),
               smallPacket = DataToPacket(smallData);
    uint16_t dataPacketSizeExcludingPayload = static_cast<uint16_t>(
        sizeof(packetTemplate.stx) +
        sizeof(packetTemplate.active) +
        sizeof(packetTemplate.payloadLength) +
        sizeof(packetTemplate.crc) +
        sizeof(packetTemplate.etx)
    );
    uint16_t expectedBigSize = static_cast<uint16_t>(dataPacketSizeExcludingPayload + sizeof(bigData)),
             expectedSmallSize = static_cast<uint16_t>(dataPacketSizeExcludingPayload + sizeof(smallData));

    uint16_t bigResult = SizeOfSerializedDataPacket(bigPacket),
             smallResult = SizeOfSerializedDataPacket(smallPacket);

    ASSERT_TEST(
        bigResult == expectedBigSize && 
        smallResult == expectedSmallSize
    );
}

void SizeOfSerializedDataPacket_MustReturnTheSameSizeAsSaveDataPacketPacketToEEPROM()
{
    uint64_t data1 = 981239;
    uint32_t data2 = 34446;
    uint8_t data3 = 123;
    DataPacket packet1 = DataToPacket(data1),
               packet2 = DataToPacket(data2),
               packet3 = DataToPacket(data3);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, packet1.payload, packet1.payloadLength);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, packet2.payload, packet2.payloadLength);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(0, packet3.payload, packet3.payloadLength);

    uint16_t resultSize1 = SizeOfSerializedDataPacket(packet1),
             resultSize2 = SizeOfSerializedDataPacket(packet2),
             resultSize3 = SizeOfSerializedDataPacket(packet3);
    uint16_t resultSavedPacketSize1,
             resultSavedPacketSize2,
             resultSavedPacketSize3;
    bool resultBool = SaveDataPacketToEEPROM(0, packet1.payload, packet1.payloadLength, &resultSavedPacketSize1) &&
                      SaveDataPacketToEEPROM(0, packet2.payload, packet2.payloadLength, &resultSavedPacketSize2) &&
                      SaveDataPacketToEEPROM(0, packet3.payload, packet3.payloadLength, &resultSavedPacketSize3);

    ASSERT_TEST(
        resultSize1 == resultSavedPacketSize1 &&
        resultSize2 == resultSavedPacketSize2 &&
        resultSize3 == resultSavedPacketSize3 &&
        resultBool == true
    );
}

void SaveDataPacketToEEPROM_SavesStxToFirstGivenAdress()
{
    uint8_t data = 42;
    uint16_t adress = 20;
    uint16_t packetSize;
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, &data, sizeof(data));

    SaveDataPacketToEEPROM(adress, &data, sizeof(data), &packetSize);

    ASSERT_TEST(EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(adress));
}

void SaveDataPacketToEEPROM_EtxIsPutDownAtTheEndOfThePacket()
{
    DataPacket packet;
    uint8_t data = 42;
    uint16_t adress = 20;
    uint16_t packetSize;
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, &data, sizeof(data));


    SaveDataPacketToEEPROM(adress, &data, sizeof(data), &packetSize);
    unsigned int expectedEtxPosition = adress + sizeof(packet.stx) + sizeof(packet.active) + sizeof(packet.payloadLength) + sizeof(packet.crc) + sizeof(data);

    ASSERT_TEST(EEPROMClass_put_param_idx_o1_v[2] == static_cast<int>(expectedEtxPosition) && EEPROMClass_put_param_t_o1_v[2] == packet.etx);
}

void SaveDataPacketToEEPROM_PacketIsCorrectlyPutDown()
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
    // This ensures that ReadDataPacketOnEEPROM returns true
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, dataPtr, sizeof(data));


    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

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

void SaveDataPacketToEEPROM_PacketWillExceedEndOfEEPROM_ReturnsCorrectPacketSize()
{
    uint32_t data = 888;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(packet);
    uint16_t eepromSize = static_cast<uint16_t>(expectedPacketSize + 5);
    uint16_t adress = static_cast<uint16_t>(eepromSize - (expectedPacketSize / 2));
    // This ensures that SaveDataPacketToEEPROM returns true
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, packet.payload, packet.payloadLength, eepromSize);

    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, packet.payload, packet.payloadLength, &packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetSize == expectedPacketSize
    );
}

void SaveDataPacketToEEPROM_PacketWillExceedEndOfEEPROM_SplitsPacketBetweenEndAndStartOfEEPROM()
{
    uint32_t data = 888;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    unsigned int expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(packet);
    uint16_t eepromSize = static_cast<uint16_t>(expectedPacketSize + 5);
    uint16_t adress = static_cast<uint16_t>(eepromSize - (expectedPacketSize / 2));
    unsigned int expectedStxAdress = static_cast<int>(adress),
                 expectedActiveFlagAdress = (expectedStxAdress + sizeof(packet.stx)) % eepromSize,
                 expectedPayloadLengthAdress = (expectedActiveFlagAdress + sizeof(packet.active)) % eepromSize,
                 expectedCRCAdress = (expectedPayloadLengthAdress + sizeof(packet.payloadLength)) % eepromSize,
                 expectedPayloadAdress = (expectedCRCAdress + sizeof(packet.crc)) % eepromSize,
                 expectedEtxAdress = (expectedPayloadAdress + sizeof(data)) % eepromSize;
    unsigned int expectedPayloadAdressPart0 = (expectedPayloadAdress) % eepromSize,
                 expectedPayloadAdressPart1 = (expectedPayloadAdress + 1) % eepromSize,
                 expectedPayloadAdressPart2 = (expectedPayloadAdress + 2) % eepromSize,
                 expectedPayloadAdressPart3 = (expectedPayloadAdress + 3) % eepromSize;
    // This ensures that SaveDataPacketToEEPROM returns true.
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, packet.payload, packet.payloadLength, eepromSize);

    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, packet.payload, packet.payloadLength, &packetSize);

    ASSERT_TEST(
        resultBool == true && 
        EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(expectedStxAdress) && EEPROMClass_put_param_t_o1_v[0] == packet.stx &&
        EEPROMClass_put_param_idx_o1_v[1] == static_cast<int>(expectedActiveFlagAdress) && EEPROMClass_put_param_t_o1_v[1] == packet.active &&
        EEPROMClass_put_param_idx_o2_v[0] == static_cast<int>(expectedPayloadLengthAdress) && EEPROMClass_put_param_t_o2_v[0] == packet.payloadLength &&
        EEPROMClass_put_param_idx_o3_v[0] == static_cast<int>(expectedCRCAdress) && EEPROMClass_put_param_t_o3_v[0] == packet.crc &&
        EEPROMClass_update_param_idx_v[0] == static_cast<int>(expectedPayloadAdressPart0) && EEPROMClass_update_param_val_v[0] == dataPtr[0] &&
        EEPROMClass_update_param_idx_v[1] == static_cast<int>(expectedPayloadAdressPart1) && EEPROMClass_update_param_val_v[1] == dataPtr[1] &&
        EEPROMClass_update_param_idx_v[2] == static_cast<int>(expectedPayloadAdressPart2) && EEPROMClass_update_param_val_v[2] == dataPtr[2] &&
        EEPROMClass_update_param_idx_v[3] == static_cast<int>(expectedPayloadAdressPart3) && EEPROMClass_update_param_val_v[3] == dataPtr[3] &&
        EEPROMClass_put_param_idx_o1_v[2] == static_cast<int>(expectedEtxAdress) && EEPROMClass_put_param_t_o1_v[2] == packet.etx &&
        packetSize == expectedPacketSize
    );
}

void SaveDataPacketToEEPROM_SplitsPacketOnPayload_PutsDownPayloadOnCorrectAdresses()
{
    uint32_t data = 888;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    uint16_t expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(packet);
    uint16_t eepromSize = static_cast<uint16_t>(expectedPacketSize + 5);
    uint16_t relativePayloadAdress = sizeof(packet.stx) + 
                                     sizeof(packet.active) +
                                     sizeof(packet.payloadLength) +
                                     sizeof(packet.crc);
    uint16_t adress = static_cast<uint16_t>(
        eepromSize - (
            relativePayloadAdress +
            (sizeof(data) / 2)
        )
    );
    uint16_t expectedPayloadAdressPart0 = static_cast<uint16_t>((adress + relativePayloadAdress + 0) % eepromSize),
             expectedPayloadAdressPart1 = static_cast<uint16_t>((adress + relativePayloadAdress + 1) % eepromSize),
             expectedPayloadAdressPart2 = static_cast<uint16_t>((adress + relativePayloadAdress + 2) % eepromSize),
             expectedPayloadAdressPart3 = static_cast<uint16_t>((adress + relativePayloadAdress + 3) % eepromSize);
    // This ensures that SaveDataPacketToEEPROM returns true.
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, packet.payload, packet.payloadLength, eepromSize);

    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, packet.payload, packet.payloadLength, &packetSize);

    ASSERT_TEST(
        resultBool == true && 
        EEPROMClass_update_param_idx_v[0] == static_cast<int>(expectedPayloadAdressPart0) && EEPROMClass_update_param_val_v[0] == dataPtr[0] &&
        EEPROMClass_update_param_idx_v[1] == static_cast<int>(expectedPayloadAdressPart1) && EEPROMClass_update_param_val_v[1] == dataPtr[1] &&
        EEPROMClass_update_param_idx_v[2] == static_cast<int>(expectedPayloadAdressPart2) && EEPROMClass_update_param_val_v[2] == dataPtr[2] &&
        EEPROMClass_update_param_idx_v[3] == static_cast<int>(expectedPayloadAdressPart3) && EEPROMClass_update_param_val_v[3] == dataPtr[3]
    );
}


void SaveDataPacketToEEPROM_PacketIsSavedButEepromFailsToReadTheData_ReturnsFalse()
{
    uint16_t data = 42;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    uint16_t adress = 20;
    DataPacket packet;

    // This ensure that SaveDataPacketToEEPROM and ReadDataPacketOnEEPROM knows the size of the eeprom.
    EEPROMClass_length_return = sizeof(data) + 20;
    // This ensures that ReadDataPacketOnEEPROM returns false
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_read_return_v.push_back(packet.active);
    EEPROMClass_get_param_t_o1_vr.push_back(sizeof(data));
    EEPROMClass_get_param_t_o2_vr.push_back(55561893);
    EEPROMClass_read_return_v.push_back(packet.etx);
    EEPROMClass_read_return_v.push_back(static_cast<uint8_t>(dataPtr[0] + 13));
    EEPROMClass_read_return_v.push_back(dataPtr[1]);

    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(resultBool == false);
}

void SaveDataPacketToEEPROM_AdaptsSizeOfPacketToFitData()
{
    DataPacket packet;
    uint16_t data = 42;
    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(&data);
    uint16_t adress = 20;
    uint16_t packetSize;
    int expectedPayloadAdress = static_cast<int>(adress + sizeof(packet.stx) + sizeof(packet.active) + sizeof(packet.payloadLength) + sizeof(packet.crc));
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, dataPtr, sizeof(data));

    SaveDataPacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

    ASSERT_TEST(EEPROMClass_update_param_idx_v[0] == expectedPayloadAdress && EEPROMClass_update_param_val_v[0] == dataPtr[0] &&
                EEPROMClass_update_param_idx_v[1] == expectedPayloadAdress + 1 && EEPROMClass_update_param_val_v[1] == dataPtr[1]);
}

void SaveDataPacketToEEPROM_AdressIsOutsideOfEEPROMsRange_ReturnsFalse()
{
    uint32_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize, adress;
    eepromSize = adress = 1024;
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, packet.payload, packet.payloadLength, eepromSize);

    uint16_t packetSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, packet.payload, packet.payloadLength, &packetSize);

    ASSERT_TEST(resultBool == false);
}

void SaveDataPacketToEEPROM_AdressIsOutsideOfEEPROMsRange_DoesNotWriteAnythingToEEPROM()
{
    uint32_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize, adress;
    eepromSize = adress = 1024;
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, packet.payload, packet.payloadLength, eepromSize);

    uint16_t packetSize;
    SaveDataPacketToEEPROM(adress, packet.payload, packet.payloadLength, &packetSize);

    ASSERT_TEST(
        EEPROMClass_put_invocations_o1 == 0 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0 &&
        EEPROMClass_update_invocations == 0
    );
}

void SaveDataPacketToEEPROM_PacketIsTooBigForEEPROM_DoesNotWriteToMemoryAndReturnsFalse()
{
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t packetSize = Helper_CalculateSizeOfPacketOnEEPROM(packet),
             adress = 0,
             eepromSize = static_cast<uint16_t>(packetSize - 1);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, packet.payload, packet.payloadLength, eepromSize);

    uint16_t resultPacketSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, packet.payload, packet.payloadLength, &resultPacketSize);

    ASSERT_TEST(
        resultBool == false &&
        EEPROMClass_put_invocations_o1 == 0 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0 &&
        EEPROMClass_update_invocations == 0
    );
}

void SaveDataPacketToEEPROM_PacketFitsOnEEPROM_ReturnsTrue()
{
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t packetSize = SizeOfSerializedDataPacket(packet),
             adress = 0,
             eepromSize = static_cast<uint16_t>(packetSize);
    Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(adress, packet.payload, packet.payloadLength, eepromSize);

    uint16_t resultPacketSize;
    bool resultBool = SaveDataPacketToEEPROM(adress, packet.payload, packet.payloadLength, &resultPacketSize);

    ASSERT_TEST(resultBool == true && resultPacketSize == packetSize);
}

// TODO: Write these tests.
// void JoinDataWithDataPacketOnEEPROM_SuccessfullyAddsTheDataToTheEndOfThePayload()
// void JoinDataWithDataPacketOnEEPROM_SuccessfullyIncreasesThePayloadLength()
// void JoinDataWithDataPacketOnEEPROM_SuccessfullyIncludesTheDataIntoTheCalculationOfTheCrc()
// void JoinDataWithDataPacketOnEEPROM_SuccessfullyAddsAnEtxToTheEndOfThePayload()
// void JoinDataWithDataPacketOnEEPROM_SuccessfullyJoinsDataWithPacket_ReturnsTrue()
// void JoinDataWithDataPacketOnEEPROM_WritesThePayloadInACircularPatternWhenExceedingTheEepromsLastAdress()
// void JoinDataWithDataPacketOnEEPROM_JoinedPacketIsValid()
// void JoinDataWithDataPacketOnEEPROM_AdressOfDataPacketIsOutsideOfTheEEPROM_ReturnsFalse()
// void JoinDataWithDataPacketOnEEPROM_AdressDoesNotPointToAnStx_ReturnsFalse()
// void JoinDataWithDataPacketOnEEPROM_CanNotFindEtx_ReturnsFalse()

void ReadDataPacketOnEEPROM_DataPacketDoesNotNeedToBeManuallyAllocatedBeforePassedToFunciton_DoesNotCrash()
{
    uint16_t data = 123;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet);

    DataPacket result;
    uint16_t packetSize;
    bool didNotCrash = false;
    bool resultBool = ReadDataPacketOnEEPROM(0, &result, &packetSize);
    didNotCrash = true;

    ASSERT_TEST(
        resultBool == true &&
        result.payload[0] == packet.payload[0] &&
        result.payload[1] == packet.payload[1] &&
        didNotCrash == true
    );
}

void ReadDataPacketOnEEPROM_ReturnsCorrectPacket() // TODO: bad test. Locks the order the mocked functions will be called. Rewrite with different framework?
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
    bool resultBool = ReadDataPacketOnEEPROM(adress, &result, &packetSizeResult);

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

void ReadDataPacketOnEEPROM_PacketIsSplitBetweenEndAndStartOfEEPROM_ReturnsCorrectPacketSize()
{
    uint32_t data = 888;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket packet = DataPacket(dataPtr, sizeof(data));
    unsigned int expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(packet);
    uint16_t eepromSize = static_cast<uint16_t>(expectedPacketSize + 5);
    uint16_t adress = static_cast<uint16_t>(eepromSize - (expectedPacketSize / 2));
    // This ensures that ReadDataPacketOnEEPROM returns true
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(adress, packet, eepromSize);

    DataPacket result;
    uint16_t packetSizeResult;
    bool resultBool = ReadDataPacketOnEEPROM(adress, &result, &packetSizeResult);

    ASSERT_TEST(
        resultBool == true &&
        expectedPacketSize == packetSizeResult
    );
}

void ReadDataPacketOnEEPROM_PacketIsSplitBetweenEndAndStartOfEEPROM_SuccessfullyReadsPacket()
{
    uint32_t data = 888;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket expectedPacket = DataPacket(dataPtr, sizeof(data));
    unsigned int expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(expectedPacket);
    uint16_t eepromSize = static_cast<uint16_t>(expectedPacketSize + 5);
    uint16_t adress = static_cast<uint16_t>(eepromSize - (expectedPacketSize / 2));
    unsigned int expectedStxAdress = static_cast<int>(adress),
                 expectedActiveFlagAdress = (expectedStxAdress + sizeof(expectedPacket.stx)) % eepromSize,
                 expectedPayloadLengthAdress = (expectedActiveFlagAdress + sizeof(expectedPacket.active)) % eepromSize,
                 expectedCRCAdress = (expectedPayloadLengthAdress + sizeof(expectedPacket.payloadLength)) % eepromSize,
                 expectedPayloadAdress = (expectedCRCAdress + sizeof(expectedPacket.crc)) % eepromSize,
                 expectedEtxAdress = (expectedPayloadAdress + sizeof(data)) % eepromSize;
    unsigned int expectedPayloadAdressPart0 = (expectedPayloadAdress) % eepromSize,
                 expectedPayloadAdressPart1 = (expectedPayloadAdress + 1) % eepromSize,
                 expectedPayloadAdressPart2 = (expectedPayloadAdress + 2) % eepromSize,
                 expectedPayloadAdressPart3 = (expectedPayloadAdress + 3) % eepromSize;
    // This ensures that ReadDataPacketOnEEPROM returns true
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(adress, expectedPacket, eepromSize);

    DataPacket result;
    uint16_t packetSizeResult;
    bool resultBool = ReadDataPacketOnEEPROM(adress, &result, &packetSizeResult);

    ASSERT_TEST(
        resultBool == true &&
        expectedPacketSize == packetSizeResult &&
        EEPROMClass_read_param_idx_v[0] == static_cast<int>(expectedStxAdress) && result.stx == expectedPacket.stx &&
        EEPROMClass_get_param_idx_o3_v[0] == static_cast<int>(expectedActiveFlagAdress) && result.active == expectedPacket.active &&
        EEPROMClass_get_param_idx_o1_v[0] == static_cast<int>(expectedPayloadLengthAdress) && result.payloadLength == expectedPacket.payloadLength &&
        EEPROMClass_get_param_idx_o2_v[0] == static_cast<int>(expectedCRCAdress) && result.crc == expectedPacket.crc &&
        EEPROMClass_read_param_idx_v[1] == static_cast<int>(expectedEtxAdress) && result.etx == expectedPacket.etx &&
        EEPROMClass_read_param_idx_v[2] == static_cast<int>(expectedPayloadAdressPart0) && result.payload[0] == expectedPacket.payload[0] &&
        EEPROMClass_read_param_idx_v[3] == static_cast<int>(expectedPayloadAdressPart1) && result.payload[1] == expectedPacket.payload[1] &&
        EEPROMClass_read_param_idx_v[4] == static_cast<int>(expectedPayloadAdressPart2) && result.payload[2] == expectedPacket.payload[2] &&
        EEPROMClass_read_param_idx_v[5] == static_cast<int>(expectedPayloadAdressPart3) && result.payload[3] == expectedPacket.payload[3]
    );
}

void ReadDataPacketOnEEPROM_PacketsPayloadIsSplitBetweenEndAndStartOfEEPROM_SuccessfullyReadsPayload()
{
    uint32_t data = 888;
    uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
    DataPacket expectedPacket = DataPacket(dataPtr, sizeof(data));
    uint16_t expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(expectedPacket);
    uint16_t eepromSize = static_cast<uint16_t>(expectedPacketSize + 5);
    uint16_t relativePayloadAdress = sizeof(expectedPacket.stx) + 
                                     sizeof(expectedPacket.active) +
                                     sizeof(expectedPacket.payloadLength) +
                                     sizeof(expectedPacket.crc);
    uint16_t adress = static_cast<uint16_t>(
        eepromSize - (
            relativePayloadAdress +
            (sizeof(data) / 2)
        )
    );
    uint16_t expectedPayloadAdressPart0 = static_cast<uint16_t>((adress + relativePayloadAdress + 0) % eepromSize),
             expectedPayloadAdressPart1 = static_cast<uint16_t>((adress + relativePayloadAdress + 1) % eepromSize),
             expectedPayloadAdressPart2 = static_cast<uint16_t>((adress + relativePayloadAdress + 2) % eepromSize),
             expectedPayloadAdressPart3 = static_cast<uint16_t>((adress + relativePayloadAdress + 3) % eepromSize);
    // This ensures that ReadDataPacketOnEEPROM returns true
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(adress, expectedPacket, eepromSize);

    DataPacket result;
    uint16_t packetSizeResult;
    bool resultBool = ReadDataPacketOnEEPROM(adress, &result, &packetSizeResult);

    ASSERT_TEST(
        resultBool == true &&
        EEPROMClass_read_param_idx_v[2] == static_cast<int>(expectedPayloadAdressPart0) && result.payload[0] == expectedPacket.payload[0] &&
        EEPROMClass_read_param_idx_v[3] == static_cast<int>(expectedPayloadAdressPart1) && result.payload[1] == expectedPacket.payload[1] &&
        EEPROMClass_read_param_idx_v[4] == static_cast<int>(expectedPayloadAdressPart2) && result.payload[2] == expectedPacket.payload[2] &&
        EEPROMClass_read_param_idx_v[5] == static_cast<int>(expectedPayloadAdressPart3) && result.payload[3] == expectedPacket.payload[3]
    );
}

void ReadDataPacketOnEEPROM_ReturnsFalseWhenAValidPacketIsNotActive()
{
    uint64_t data = 123211321;
    DataPacket packet = DataPacket(reinterpret_cast<uint8_t *>(&data), sizeof(data));
    packet.active = 0x00;

    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet);

    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ReadDataPacketOnEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
}

void ReadDataPacketOnEEPROM_ReturnsTrueWhenAValidPacketIsActive()
{
    uint64_t data = 123211321;
    DataPacket packet = DataPacket(reinterpret_cast<uint8_t *>(&data), sizeof(data)); // packet.active is set to be active by default.

    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, packet);

    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ReadDataPacketOnEEPROM(0, &result, &packetSize);

    ASSERT_TEST(resultBool == true);
}

void ReadDataPacketOnEEPROM_EepromReturnsFaultyData_ReturnsFalse() // TODO: bad test. Locks the order the mocked functions will be called. Rewrite with different framework?
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
    bool resultBool = ReadDataPacketOnEEPROM(adress, &result, &packetSize);

    ASSERT_TEST(resultBool == false);
}

void ReadDataPacketOnEEPROM_AdressIsOutOfEEPROMsRange_ReturnsFalse()
{
    uint32_t data = 544;
    DataPacket packet = DataToPacket(data);
    uint16_t adress, eepromSize;
    eepromSize = adress = 1024;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(adress, packet, eepromSize);

    DataPacket result;
    uint16_t packetSize;
    bool resultBool = ReadDataPacketOnEEPROM(adress, &result, &packetSize);

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

void DeactivatePacket_OverritesCorrectByteWithADeactivatedFlag()
{
    uint16_t adress = 23;
    uint16_t deactivatedFlag = 0x00;
    DataPacket expectedPacket;
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>(adress + sizeof(expectedPacket.stx));
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);

    DeactivatePacket(adress);

    ASSERT_TEST(
        EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress &&
        EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag
    );
}

void DeactivatePacket_ReadsStxPayloadLengthAndEtxFromTheRightPlaces()
{
    uint16_t adress = 23;
    uint16_t deactivatedFlag = 0x00;
    uint32_t data = 5;
    DataPacket expectedPacket = DataToPacket(data);
    uint16_t expectedStxAdress = static_cast<uint16_t>(adress);
    uint16_t expectedPayloadAdress = static_cast<uint16_t>(adress + sizeof(expectedPacket.stx) + sizeof(expectedPacket.active));
    uint16_t expectedEtxAdress = static_cast<uint16_t>(adress + sizeof(expectedPacket.stx) + sizeof(expectedPacket.active) + sizeof(expectedPacket.payloadLength) + sizeof(expectedPacket.crc) + sizeof(data));
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);

    DeactivatePacket(adress);

    ASSERT_TEST(
        EEPROMClass_read_param_idx_v[0] == expectedStxAdress &&
        EEPROMClass_get_param_idx_o1_v[0] == expectedPayloadAdress &&
        EEPROMClass_read_param_idx_v[1] == expectedEtxAdress
    );
}

void DeactivatePacket_DeactivatesPacketSuccessfully_ReturnsTrue()
{
    uint16_t adress = 23;
    uint16_t deactivatedFlag = 0x00;
    DataPacket expectedPacket;
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>(adress + sizeof(expectedPacket.stx));
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);

    bool result = DeactivatePacket(adress);

    ASSERT_TEST(
        result == true &&
        EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress &&
        EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag
    );
}

void DeactivatePacket_StxIsAtTheLastAdressAndActiveFlagIsOnTheFirstAdressOfEeprom_DeactivatesTheCorrectAdress()
{
    uint16_t eepromSize = 23;
    EEPROMClass_length_return = eepromSize;
    uint16_t deactivatedFlag = 0x00;
    uint32_t data = 15;
    DataPacket packet = DataToPacket(data);
    uint16_t adress = static_cast<uint16_t>(eepromSize - sizeof(packet.stx));
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>((adress + sizeof(packet.stx)) % eepromSize);
    uint16_t expectedStxAdress = static_cast<uint16_t>(adress);
    uint16_t expectedPayloadAdress = static_cast<uint16_t>((adress + 
                                                            sizeof(packet.stx) + 
                                                            sizeof(packet.active)
                                                            ) % eepromSize);
    uint16_t expectedEtxAdress = static_cast<uint16_t>((adress + 
                                                        sizeof(packet.stx) + 
                                                        sizeof(packet.active) + 
                                                        sizeof(packet.payloadLength) + 
                                                        sizeof(packet.crc) + 
                                                        sizeof(data)
                                                        ) % eepromSize);
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(packet.etx);

    DeactivatePacket(adress);

    ASSERT_TEST(
        expectedOverwrittenAdress == 0 &&
        EEPROMClass_read_param_idx_v[0] == expectedStxAdress &&
        EEPROMClass_get_param_idx_o1_v[0] == expectedPayloadAdress &&
        EEPROMClass_read_param_idx_v[1] == expectedEtxAdress &&
        EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress &&
        EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag
    );
}

void DeactivatePacket_AdressDoesNotPointToStx_ReturnsFalse()
{
    uint16_t adress = 23;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(0x00);

    bool result = DeactivatePacket(adress);

    ASSERT_TEST(result == false);
}

void DeactivatePacket_AdressDoesNotPointToStx_DoesNotWriteToEEPROM()
{
    uint16_t adress = 23;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(0x00);

    bool result = DeactivatePacket(adress);

    ASSERT_TEST(
        result == false &&
        EEPROMClass_put_invocations_o1 == 0
    );
}

void DeactivatePacket_AdressIsOutOfRangeOfEeprom_ReturnsFalseAndDoesNotWriteToEeprom()
{
    uint16_t eepromSize = 23;
    EEPROMClass_length_return = eepromSize;
    uint16_t adress = eepromSize;
    uint16_t deactivatedFlag = 0x00;
    uint32_t data = 15;
    DataPacket packet = DataToPacket(data);
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = DeactivatePacket(adress);

    ASSERT_TEST(
        resultBool == false &&
        EEPROMClass_put_invocations_o1 == 0
    );
}


void DeactivatePacket_AdressPointsToStxButCantFindEtxOfPacket_ReturnsFalse()
{
    uint16_t adress = 23;
    DataPacket packet;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(0x00);

    bool result = DeactivatePacket(adress);

    ASSERT_TEST(
        result == false
    );
}

void DeactivatePacket_AdressPointsToStxButCantFindEtxOfPacket_DoesNotWriteToEEPROM()
{
    uint16_t adress = 23;
    DataPacket packet;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(0x00);

    bool result = DeactivatePacket(adress);

    ASSERT_TEST(
        result == false &&
        EEPROMClass_put_invocations_o1 == 0
    );
}

void FindFirstDataPacketOnEEPROM_TakesInAStartAdress_BeginsLookingAtTheGivenAdress()
{
    uint16_t startAdress = 13;
    uint16_t eepromSize = 100;
    EEPROMClass_length_return = eepromSize;

    DataPacket result;
    uint16_t packetSize, packetAdress;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &result, &packetSize, &packetAdress);

    ASSERT_TEST(EEPROMClass_read_param_idx_v[0] == startAdress);
}

void FindFirstDataPacketOnEEPROM_FindsPacket_ReturnsTrue()
{
    uint16_t startAdress = 2;
    uint16_t eepromSize = 40;
    EEPROMClass_length_return = eepromSize;
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(startAdress, expectedPacket, eepromSize);

    DataPacket result;
    uint16_t packetSize, packetAdress;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &result, &packetSize, &packetAdress);

    ASSERT_TEST(resultBool == true);
}

void FindFirstDataPacketOnEEPROM_PacketIsPresentAfterGarbageData_FindsPacket()
{
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    uint16_t expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(expectedPacket);
    uint16_t startAdress = 2;
    uint16_t expectedAdress = static_cast<uint16_t>(startAdress + 2);
    uint16_t eepromSize = 40;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(9);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress, expectedPacket, eepromSize);

    DataPacket result;
    uint16_t resultPacketSize, resultPacketAdress;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &result, &resultPacketSize, &resultPacketAdress);

    ASSERT_TEST(
        resultBool == true &&
        resultPacketAdress == expectedAdress &&
        result.stx == expectedPacket.stx &&
        result.active == expectedPacket.active &&
        result.payloadLength == expectedPacket.payloadLength &&
        result.crc == expectedPacket.crc &&
        result.payload[0] == expectedPacket.payload[0] && 
        result.etx == expectedPacket.etx &&
        resultPacketSize == expectedPacketSize
    );
}

void FindFirstDataPacketOnEEPROM_PacketIsPresentAfterGarbageData_FindsPacketOnCorrectAdress()
{
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    uint16_t startAdress = 2;
    uint16_t expectedAdress = static_cast<uint16_t>(startAdress + 2);
    uint16_t eepromSize = 40;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(9);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress, expectedPacket, eepromSize);

    DataPacket result;
    uint16_t packetSize, packetAdress;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &result, &packetSize, &packetAdress);

    ASSERT_TEST(
        resultBool == true &&
        packetAdress == expectedAdress
    );
}

void FindFirstDataPacketOnEEPROM_FindsPacket_ReturnsCorrectPacketSize()
{
    uint16_t startAdress = 2;
    uint16_t eepromSize = 40;
    EEPROMClass_length_return = eepromSize;
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    uint16_t expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(expectedPacket);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(startAdress, expectedPacket, eepromSize);

    DataPacket result;
    uint16_t packetSize, packetAdress;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &result, &packetSize, &packetAdress);

    ASSERT_TEST(
        resultBool == true &&
        packetSize == expectedPacketSize
    );
}

void FindFirstDataPacketOnEEPROM_NoPacketIsPresent_LooksAtEveryPositionOfTheEeprom()
{
    uint16_t startAdress = 3,
            secondAdress = 0,
            thirdAdress = 1,
            fourthAdress = 2;
    uint16_t eepromSize = 4;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return = 0x00;

    DataPacket result;
    uint16_t packetSize, packetAdress;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &result, &packetSize, &packetAdress);

    ASSERT_TEST(
        EEPROMClass_read_param_idx_v[0] == startAdress &&
        EEPROMClass_read_param_idx_v[1] == secondAdress &&
        EEPROMClass_read_param_idx_v[2] == thirdAdress &&
        EEPROMClass_read_param_idx_v[3] == fourthAdress &&
        EEPROMClass_read_invocations == eepromSize
    );
}

void FindFirstDataPacketOnEEPROM_NoPacketIsPresent_ReturnsFalse()
{
    uint16_t startAdress = 3;
    uint16_t eepromSize = 4;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return = 0x00;

    DataPacket result;
    uint16_t packetSize, packetAdress;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &result, &packetSize, &packetAdress);

    ASSERT_TEST(resultBool == false);
}

void FindFirstDataPacketOnEEPROM_PacketIsPresentOnEEPROMButStartAdressIsOutOfRange_ReturnsFalse()
{
    uint16_t eepromSize = 40;
    uint16_t startAdress = eepromSize;
    EEPROMClass_length_return = eepromSize;
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(0, expectedPacket, eepromSize);

    DataPacket result;
    uint16_t packetSize, packetAdress;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &result, &packetSize, &packetAdress);

    ASSERT_TEST(resultBool == false);
}

void DeactivateAllPacketsOnEEPROM_OnePacketIsPresent_OverwritesPacketWithADeactivatedFlag()
{
    uint32_t data = 2020;
    DataPacket packet = DataToPacket(data);
    uint16_t expectedAdress = 0;
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>(expectedAdress + sizeof(packet.stx));
    uint8_t deactivatedFlag = 0x00;
    uint16_t eepromSize = 50;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress, packet, eepromSize);
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = DeactivateAllPacketsOnEEPROM();

    ASSERT_TEST(
        EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress &&
        EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag
    );
}

void DeactivateAllPacketsOnEEPROM_OverwritesPacketWithADeactivatedFlag_ReturnsTrue()
{
    uint32_t data = 2020;
    DataPacket packet = DataToPacket(data);
    uint16_t expectedAdress = 0;
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>(expectedAdress + sizeof(packet.stx));
    uint8_t deactivatedFlag = 0x00;
    uint16_t eepromSize = 50;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress, packet, eepromSize);
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = DeactivateAllPacketsOnEEPROM();

    ASSERT_TEST(
        resultBool == true &&
        EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress &&
        EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag
    );
}

void DeactivateAllPacketsOnEEPROM_ADeactivatedPacketIsPresent_DoesNotOverwritePacket()
{
    uint8_t deactivatedFlag = 0x00;
    uint16_t expectedAdress = 0;
    uint32_t data = 2020;
    DataPacket packet = DataToPacket(data);
    packet.active = deactivatedFlag;
    uint16_t eepromSize = 50;
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress, packet, eepromSize);
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = DeactivateAllPacketsOnEEPROM();

    ASSERT_TEST(
        EEPROMClass_put_invocations_o1 == 0
    );
}

void DeactivateAllPacketsOnEEPROM_OnePacketIsPresentAfterGarbageData_OverwritesCorrectPositionWithADeactivatedFlag()
{
    uint32_t data = 2020;
    DataPacket packet = DataToPacket(data);
    uint16_t expectedAdress = 4;
    uint16_t expectedOverwrittenAdress = static_cast<uint16_t>(expectedAdress + sizeof(packet.stx));
    uint8_t deactivatedFlag = 0x00;
    uint16_t eepromSize = 50;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(0x23);
    EEPROMClass_read_return_v.push_back(0x60);
    EEPROMClass_read_return_v.push_back(0x30);
    EEPROMClass_read_return_v.push_back(0x71);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress, packet, eepromSize);
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = DeactivateAllPacketsOnEEPROM();

    ASSERT_TEST(
        EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress &&
        EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag
    );
}

void DeactivateAllPacketsOnEEPROM_MultiplePacketsArePresent_OverwritesAllPacketsWithADeactivatedFlag()
{
    uint8_t deactivatedFlag = 0x00;
    // Set up packets
    uint32_t data1 = 220,
             data2 = 1010,
             data3 = 303;
    DataPacket packet1 = DataToPacket(data1);
    DataPacket packet2 = DataToPacket(data2);
    DataPacket packet3 = DataToPacket(data3);
    uint16_t expectedAdress1 = 0,
             expectedAdress2 = static_cast<uint16_t>(expectedAdress1 + Helper_CalculateSizeOfPacketOnEEPROM(packet1)),
             expectedAdress3 = static_cast<uint16_t>(expectedAdress2 + Helper_CalculateSizeOfPacketOnEEPROM(packet2));
    uint16_t expectedOverwrittenAdress1 = static_cast<uint16_t>(expectedAdress1 + sizeof(packet1.stx)),
             expectedOverwrittenAdress2 = static_cast<uint16_t>(expectedAdress2 + sizeof(packet2.stx)),
             expectedOverwrittenAdress3 = static_cast<uint16_t>(expectedAdress3 + sizeof(packet3.stx));
    // Set up mocked behaviour
    uint16_t eepromSize = static_cast<uint16_t>(expectedAdress3 + Helper_CalculateSizeOfPacketOnEEPROM(packet3) + 10);
    EEPROMClass_length_return = eepromSize;
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress1, packet1, eepromSize);
    EEPROMClass_read_return_v.push_back(packet1.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet1.payloadLength);
    EEPROMClass_read_return_v.push_back(packet1.etx);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress2, packet2, eepromSize);
    EEPROMClass_read_return_v.push_back(packet2.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet2.payloadLength);
    EEPROMClass_read_return_v.push_back(packet2.etx);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress3, packet3, eepromSize);
    EEPROMClass_read_return_v.push_back(packet3.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet3.payloadLength);
    EEPROMClass_read_return_v.push_back(packet3.etx);

    bool resultBool = DeactivateAllPacketsOnEEPROM();

    ASSERT_TEST(
        EEPROMClass_put_param_idx_o1_v[0] == expectedOverwrittenAdress1 &&
        EEPROMClass_put_param_t_o1_v[0] == deactivatedFlag &&
        EEPROMClass_put_param_idx_o1_v[1] == expectedOverwrittenAdress2 &&
        EEPROMClass_put_param_t_o1_v[1] == deactivatedFlag &&
        EEPROMClass_put_param_idx_o1_v[2] == expectedOverwrittenAdress3 &&
        EEPROMClass_put_param_t_o1_v[2] == deactivatedFlag
    );
}

void DeactivateAllPacketsOnEEPROM_NoPacketIsPresent_DoesNotWriteToEEPROM()
{
    uint8_t deactivatedFlag = 0x00;
    uint16_t eepromSize = 50;
    EEPROMClass_length_return = eepromSize;

    bool resultBool = DeactivateAllPacketsOnEEPROM();

    ASSERT_TEST(
        EEPROMClass_put_invocations_o1 == 0
    );
}

void DeactivateAllPacketsOnEEPROM_NoPacketIsPresent_ReturnsFalse()
{
    uint8_t deactivatedFlag = 0x00;
    uint16_t eepromSize = 50;
    EEPROMClass_length_return = eepromSize;

    bool resultBool = DeactivateAllPacketsOnEEPROM();

    ASSERT_TEST(resultBool == false);
}

// void FindFirstDataPacketOnEEPROM_TwoPacketsArePresent_StartAdressIsPutInBetweenPackages_FindsSecondPacketFirst(); // Note: can't be tested without somehow linking the idx to the output of the mocked function.
// void SaveDataPacketToEEPROM_PacketWillExceedEndOfEEPROM_SplitsPacketOnDataTypeBiggerThan1Byte_SuccessfullySplitsPacketWithoutLoosingData(); // Note: Can't be tested. Would test the eeprom library. 
