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

void CalculateCRC_DoesNotAlterInput()
{
    uint8_t data1 = 0xF,
            data2 = 0xE,
            *dataPtr1 = &data1,
            *dataPtr2 = &data2;
    uint8_t expectedData1 = uint8_t(data1),
            expectedData2 = uint8_t(data2),
            *expectedDataPtr1 = dataPtr1,
            *expectedDataPtr2 = dataPtr2;

    uint32_t crc = CalculateCRC(dataPtr1, sizeof(data1));
    uint32_t expectedCrc = crc;
    CalculateCRC(dataPtr2, sizeof(data2), crc);
    
    ASSERT_TEST(
        data1 == expectedData1 &&
        data2 == expectedData2 &&
        dataPtr1 == expectedDataPtr1 &&
        dataPtr2 == expectedDataPtr2 &&
        crc == expectedCrc
    );
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

void SizeOfEmptySerializedDataPacket_ReturnsTheAmountOfBytesAnEmptyDataPacketWouldOccupyOnMemory()
{
    DataPacket packet;
    uint16_t expectedSize = static_cast<uint16_t>(
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc) +
        sizeof(packet.etx)
    );

    uint16_t result = SizeOfEmptySerializedDataPacket();

    ASSERT_TEST(result == expectedSize);
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

void ReadBytesFromEEPROM_BeginsReadingAtTheAdress()
{
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return = 0;
    const uint16_t adress = 9,
             amountOfBytes = 1;

    uint8_t result[amountOfBytes];
    ReadBytesFromEEPROM(adress, amountOfBytes, result);

    ASSERT_TEST(EEPROMClass_read_param_idx_v.at(0) == adress);
}

void ReadBytesFromEEPROM_ReadsTheRequestedAmountOfBytes()
{
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return = 0;
    const uint16_t adress = 9,
             amountOfBytes = 7;

    uint8_t result[amountOfBytes];
    ReadBytesFromEEPROM(adress, amountOfBytes, result);

    ASSERT_TEST(EEPROMClass_read_invocations == amountOfBytes);
}

void ReadBytesFromEEPROM_ReadsBytesFromTheCorrectAdressUsingACyclicPattern()
{
    uint16_t eepromSize = 10;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return = 0;
    const uint16_t adress = static_cast<uint16_t>(eepromSize - 2),
             amountOfBytes = 6;
    uint16_t expectedReadAdress0 = CyclicAdress(adress + 0, eepromSize),
             expectedReadAdress1 = CyclicAdress(adress + 1, eepromSize), 
             expectedReadAdress2 = CyclicAdress(adress + 2, eepromSize), 
             expectedReadAdress3 = CyclicAdress(adress + 3, eepromSize), 
             expectedReadAdress4 = CyclicAdress(adress + 4, eepromSize), 
             expectedReadAdress5 = CyclicAdress(adress + 5, eepromSize);

    uint8_t result[amountOfBytes];
    ReadBytesFromEEPROM(adress, amountOfBytes, result);

    ASSERT_TEST(
        EEPROMClass_read_param_idx_v.at(0) == expectedReadAdress0 &&
        EEPROMClass_read_param_idx_v.at(1) == expectedReadAdress1 &&
        EEPROMClass_read_param_idx_v.at(2) == expectedReadAdress2 &&
        EEPROMClass_read_param_idx_v.at(3) == expectedReadAdress3 &&
        EEPROMClass_read_param_idx_v.at(4) == expectedReadAdress4 &&
        EEPROMClass_read_param_idx_v.at(5) == expectedReadAdress5
    );
}

void ReadBytesFromEEPROM_FillsTheOutputArrayWithTheCorrectBytesFromEEPROM()
{
    uint16_t eepromSize = 10;
    EEPROMClass_length_return = eepromSize;
    const uint16_t adress = static_cast<uint16_t>(eepromSize - 2),
             amountOfBytes = 4;
    uint16_t expectedReadAdress0 = CyclicAdress(adress + 0, eepromSize),
             expectedReadAdress1 = CyclicAdress(adress + 1, eepromSize), 
             expectedReadAdress2 = CyclicAdress(adress + 2, eepromSize), 
             expectedReadAdress3 = CyclicAdress(adress + 3, eepromSize);
    uint8_t expectedResult0 = 6,
             expectedResult1 = 5,
             expectedResult2 = 4,
             expectedResult3 = 3;
    EEPROMClass_read_return_v.push_back(expectedResult0);
    EEPROMClass_read_return_v.push_back(expectedResult1);
    EEPROMClass_read_return_v.push_back(expectedResult2);
    EEPROMClass_read_return_v.push_back(expectedResult3);

    uint8_t result[amountOfBytes];
    ReadBytesFromEEPROM(adress, amountOfBytes, result);

    ASSERT_TEST(
        EEPROMClass_read_param_idx_v.at(0) == expectedReadAdress0 && result[0] == expectedResult0 && 
        EEPROMClass_read_param_idx_v.at(1) == expectedReadAdress1 && result[1] == expectedResult1 && 
        EEPROMClass_read_param_idx_v.at(2) == expectedReadAdress2 && result[2] == expectedResult2 && 
        EEPROMClass_read_param_idx_v.at(3) == expectedReadAdress3 && result[3] == expectedResult3
    );
}

void ReadBytesFromEEPROM_FillsTheOutputArrayWithTheRequestedAmountOfBytes()
{
    const uint16_t adress = 0,
             amountOfRequestedBytes = 3;
    uint8_t expectedResult0 = 6,
            expectedResult1 = 5,
            expectedResult2 = 4;
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return_v.push_back(expectedResult0);
    EEPROMClass_read_return_v.push_back(expectedResult1);
    EEPROMClass_read_return_v.push_back(expectedResult2);
    EEPROMClass_read_return_v.push_back(3);
    EEPROMClass_read_return_v.push_back(2);
    EEPROMClass_read_return_v.push_back(1);

    uint8_t result[6] = {0, 0, 0, 0, 0, 0};
    ReadBytesFromEEPROM(adress, amountOfRequestedBytes, result);

    ASSERT_TEST(
        result[0] == expectedResult0 && 
        result[1] == expectedResult1 && 
        result[2] == expectedResult2 &&
        result[3] == 0 &&
        result[4] == 0 &&
        result[5] == 0
    );
}

void ReadBytesFromEEPROM_SuccessfullyReadsASeriesOfBytes_ReturnsTrue()
{
    uint16_t eepromSize = 10;
    EEPROMClass_length_return = eepromSize;
    const uint16_t adress = static_cast<uint16_t>(eepromSize - 1),
             amountOfBytes = 3;
    uint16_t expectedReadAdress0 = CyclicAdress(adress + 0, eepromSize),
             expectedReadAdress1 = CyclicAdress(adress + 1, eepromSize), 
             expectedReadAdress2 = CyclicAdress(adress + 2, eepromSize);
    uint8_t expectedResult0 = 6,
             expectedResult1 = 5,
             expectedResult2 = 4;
    EEPROMClass_read_return_v.push_back(expectedResult0);
    EEPROMClass_read_return_v.push_back(expectedResult1);
    EEPROMClass_read_return_v.push_back(expectedResult2);

    uint8_t result[amountOfBytes];
    bool resultBool = ReadBytesFromEEPROM(adress, amountOfBytes, result);

    ASSERT_TEST(
        EEPROMClass_read_param_idx_v.at(0) == expectedReadAdress0 && result[0] == expectedResult0 && 
        EEPROMClass_read_param_idx_v.at(1) == expectedReadAdress1 && result[1] == expectedResult1 && 
        EEPROMClass_read_param_idx_v.at(2) == expectedReadAdress2 && result[2] == expectedResult2 &&
        resultBool == true
    );
}

void ReadBytesFromEEPROM_AdressIsOutOfRangeOnEEPROM_DoesNotReadAndReturnsFalse()
{
    uint16_t eepromSize = 5;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return = 0;
    const uint16_t adress = eepromSize,
                   amountOfBytes = 1;

    uint8_t result[amountOfBytes];
    bool resultBool = ReadBytesFromEEPROM(adress, amountOfBytes, result);

    ASSERT_TEST(
        resultBool == false && 
        EEPROMClass_read_invocations == 0
    );
}

void ReadBytesFromEEPROM_TriesToReadMoreBytesThanWhatFitsOnTheEEPROM_DoesNotReadAndReturnsFalse()
{
    const uint16_t adress = 0,
                   amountOfBytes = 6;
    uint16_t eepromSize = amountOfBytes - 1;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return = 0;

    uint8_t result[amountOfBytes];
    bool resultBool = ReadBytesFromEEPROM(adress, amountOfBytes, result);

    ASSERT_TEST(
        resultBool == false && 
        EEPROMClass_read_invocations == 0
    );
}

void ReadBytesFromEEPROM_TriesToReadZeroBytes_DoesNotReadAndReturnsFalse()
{
    const uint16_t adress = 0,
                   amountOfBytes = 0;
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return = 0;

    uint8_t result[1];
    bool resultBool = ReadBytesFromEEPROM(adress, amountOfBytes, result);

    ASSERT_TEST(
        resultBool == false && 
        EEPROMClass_read_invocations == 0
    );
}

void IsPacketOnEEPROMValid_BeginsReadingAtTheAdress()
{
    uint16_t adress = 75;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);
    uint16_t expectedReadAdress = adress;

    IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(EEPROMClass_read_param_idx_v.at(0) == expectedReadAdress);
}

void IsPacketOnEEPROMValid_PacketIsValid_ReturnsTrue()
{
    uint16_t adress = 3;
    uint16_t data = 165;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);
    int32_t stxAdress = adress,
             activeFlagAdress = stxAdress + sizeof(packet.stx),
             payloadLengthAdress = activeFlagAdress + sizeof(packet.active),
             crcAdress = payloadLengthAdress + sizeof(packet.payloadLength),
             payloadAdress = crcAdress + sizeof(packet.crc),
             adressPayload0 = payloadAdress,
             adressPayload1 = payloadAdress + 1,
             etxAdress = payloadAdress + packet.payloadLength;
    
    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(
        resultBool == true && 
        EEPROMClass_read_param_idx_v.at(0) == stxAdress &&
        EEPROMClass_get_param_idx_o3_v.at(0) == activeFlagAdress &&
        EEPROMClass_get_param_idx_o1_v.at(0) == payloadLengthAdress &&
        EEPROMClass_read_param_idx_v.at(1) == etxAdress &&
        EEPROMClass_read_param_idx_v.at(2) == adressPayload0 &&
        EEPROMClass_read_param_idx_v.at(3) == adressPayload1 &&
        EEPROMClass_get_param_idx_o2_v.at(0) == crcAdress
    );
}

void IsPacketOnEEPROMValid_ReadsThePacketInACylicFormat()
{
    uint16_t data = 165;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = SizeOfSerializedDataPacket(packet);
    EEPROMClass_length_return = eepromSize;
    uint16_t adress = static_cast<uint16_t>(eepromSize - sizeof(packet.stx));
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);
    uint16_t stxAdress = adress,
             activeFlagAdress = CyclicAdress(stxAdress + sizeof(packet.stx), eepromSize),
             payloadLengthAdress = CyclicAdress(activeFlagAdress + sizeof(packet.active), eepromSize),
             crcAdress = CyclicAdress(payloadLengthAdress + sizeof(packet.payloadLength), eepromSize),
             payloadAdress = CyclicAdress(crcAdress + sizeof(packet.crc), eepromSize),
             adressPayload0 = CyclicAdress(payloadAdress, eepromSize),
             adressPayload1 = CyclicAdress(payloadAdress + 1, eepromSize),
             etxAdress = CyclicAdress(payloadAdress + packet.payloadLength, eepromSize);
    
    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(
        resultBool == true &&
        EEPROMClass_read_param_idx_v.at(0) == stxAdress &&
        EEPROMClass_get_param_idx_o3_v.at(0) == activeFlagAdress &&
        EEPROMClass_get_param_idx_o1_v.at(0) == payloadLengthAdress &&
        EEPROMClass_read_param_idx_v.at(1) == etxAdress &&
        EEPROMClass_read_param_idx_v.at(2) == adressPayload0 &&
        EEPROMClass_read_param_idx_v.at(3) == adressPayload1 &&
        EEPROMClass_get_param_idx_o2_v.at(0) == crcAdress
    );
}

void IsPacketOnEEPROMValid_WithOutputArguments_ReturnsPayloadsAdress()
{
    uint16_t adress = 75;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);
    uint16_t expectedPayloadAdress = static_cast<uint16_t>(
        adress +
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc)
    );

    uint16_t resultPacketSize, resultPayloadAdress, resultPayloadLength;
    IsPacketOnEEPROMValid(adress, &resultPacketSize, &resultPayloadAdress, &resultPayloadLength);

    ASSERT_TEST(resultPayloadAdress == expectedPayloadAdress);
}

void IsPacketOnEEPROMValid_WithOutputArguments_ReturnsThePayloadsLength()
{
    uint16_t adress = 75;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);
    uint16_t expectedPayloadLength = packet.payloadLength;

    uint16_t resultPacketSize, resultPayloadAdress, resultPayloadLength;
    IsPacketOnEEPROMValid(adress, &resultPacketSize, &resultPayloadAdress, &resultPayloadLength);

    ASSERT_TEST(resultPayloadLength == expectedPayloadLength);
}

void IsPacketOnEEPROMValid_WithOutputArguments_ReturnsTheCorrectPacketSize()
{
    uint16_t adress = 7;
    uint16_t data = 2;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);
    uint16_t expectedPacketSize = SizeOfSerializedDataPacket(packet);

    uint16_t resultPacketSize, resultPayloadAdress, resultPayloadLength;
    IsPacketOnEEPROMValid(adress, &resultPacketSize, &resultPayloadAdress, &resultPayloadLength);

    ASSERT_TEST(resultPacketSize == expectedPacketSize);
}

void IsPacketOnEEPROMValid_AdressIsOutsideOfTheEEPROMsRange_ReturnsFalse()
{
    uint16_t eepromSize = 100;
    EEPROMClass_length_return = eepromSize;
    uint16_t adress = eepromSize;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);

    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(resultBool == false);
}

void IsPacketOnEEPROMValid_CanNotFindStx_ReturnsFalse()
{
    uint16_t adress = 0;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    // Prepare to return a packet without stx.
    EEPROMClass_read_return_v.push_back(0);
    EEPROMClass_get_param_t_o3_vr.push_back(packet.active);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_read_return_v.push_back(packet.etx);
    for (int i = 0; i < packet.payloadLength; i++) 
    {
        EEPROMClass_read_return_v.push_back(packet.payload[i]);
    }
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);

    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(resultBool == false);
}

void IsPacketOnEEPROMValid_PacketIsDeactivated_ReturnsFalse()
{
    uint8_t deactivatedFlag = 0x00;
    uint16_t adress = 0;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    packet.active = deactivatedFlag;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);

    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(resultBool == false);
}

void IsPacketOnEEPROMValid_PayloadLengthIsZero_ReturnsFalse()
{
    uint16_t adress = 0;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    packet.payloadLength = 0;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);

    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(resultBool == false);
}

void IsPacketOnEEPROMValid_PacketFitsOnEEPROM_ReturnsTrue()
{
    uint16_t adress = 0;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = SizeOfSerializedDataPacket(packet);
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);

    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(resultBool == true);
}

void IsPacketOnEEPROMValid_PayloadLengthIsLargerThanWhatFitsOnTheEEPROM_ReturnsFalse()
{
    uint16_t adress = 0;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = static_cast<uint16_t>(SizeOfSerializedDataPacket(packet) - 1);
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);

    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(resultBool == false);
}

void IsPacketOnEEPROMValid_CanNotFindEtx_ReturnsFalse()
{
    uint16_t adress = 0;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o3_vr.push_back(packet.active);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    // Prepare to return a packet without etx.
    EEPROMClass_read_return_v.push_back(0);
    for (int i = 0; i < packet.payloadLength; i++) 
    {
        EEPROMClass_read_return_v.push_back(packet.payload[i]);
    }
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);

    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(resultBool == false);
}

void IsPacketOnEEPROMValid_CrcDoesNotMatchPayload_ReturnsFalse()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t adress = 0;
    uint16_t data = 13;
    DataPacket packet = DataToPacket(data);
    uint32_t faultyCrc = packet.crc + 12;
    packet.crc = faultyCrc;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, packet, eepromSize);

    bool resultBool = IsPacketOnEEPROMValid(adress);

    ASSERT_TEST(resultBool == false);
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

void JoinDataWithDataPacketOnEEPROM_ReadsThePacketUsingACircularPatternAndReadsFromTheCorrectAdress()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = static_cast<uint16_t>(
        SizeOfSerializedDataPacket(packet) +
        sizeof(dataToAdd)
    );
    uint16_t adress = static_cast<uint16_t>(
        eepromSize - sizeof(packet.stx)
    );
    int32_t expectedStxAdress = adress;
    int32_t expectedPayloadLengthAdress = CyclicAdress(
        (
            expectedStxAdress +
            sizeof(packet.stx) +
            sizeof(packet.active)
        )
        , eepromSize
    );
    int32_t expectedCrcAdress = CyclicAdress(
        (
            expectedPayloadLengthAdress +
            sizeof(packet.payloadLength)
        )
        , eepromSize
    );
    int32_t expectedEtxAdress = CyclicAdress(
        (
            expectedCrcAdress +
            sizeof(packet.crc) + 
            sizeof(packet.payload[0]) * packet.payloadLength
        )
        , eepromSize
    );
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        EEPROMClass_read_param_idx_v.at(0) == expectedStxAdress &&
        EEPROMClass_get_param_idx_o1_v.at(0) == expectedPayloadLengthAdress &&
        EEPROMClass_read_param_idx_v.at(1) == expectedEtxAdress &&
        EEPROMClass_get_param_idx_o2_v.at(0) == expectedCrcAdress
    );
}

void JoinDataWithDataPacketOnEEPROM_SuccessfullyAddsTheDataToTheEndOfThePayload()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t adress = 13;
    int32_t expectedDataStartAdress = (
                adress +
                sizeof(packet.stx) +
                sizeof(packet.active) +
                sizeof(packet.payloadLength) +
                sizeof(packet.crc) +
                sizeof(packet.payload[0]) * packet.payloadLength
             ),
             expectedDataAdress0 = expectedDataStartAdress + 0,
             expectedDataAdress1 = expectedDataStartAdress + 1,
             expectedDataAdress2 = expectedDataStartAdress + 2,
             expectedDataAdress3 = expectedDataStartAdress + 3,
             expectedNewEtxAdress = expectedDataStartAdress + 4;
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        EEPROMClass_update_param_idx_v.at(0) == expectedDataAdress0 && EEPROMClass_update_param_val_v.at(0) == dataToAddPtr[0] &&
        EEPROMClass_update_param_idx_v.at(1) == expectedDataAdress1 && EEPROMClass_update_param_val_v.at(1) == dataToAddPtr[1] &&
        EEPROMClass_update_param_idx_v.at(2) == expectedDataAdress2 && EEPROMClass_update_param_val_v.at(2) == dataToAddPtr[2] &&
        EEPROMClass_update_param_idx_v.at(3) == expectedDataAdress3 && EEPROMClass_update_param_val_v.at(3) == dataToAddPtr[3]
    );
}

void JoinDataWithDataPacketOnEEPROM_SuccessfullyAddsAnEtxToTheEndOfThePayload()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t adress = 13;
    int32_t expectedNewEtxAdress = (
        adress +
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc) +
        sizeof(packet.payload[0]) * packet.payloadLength +
        sizeof(dataToAdd)
    );
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        EEPROMClass_update_param_idx_v.at(4) == expectedNewEtxAdress && 
        EEPROMClass_update_param_val_v.at(4) == packet.etx
    );
}

void JoinDataWithDataPacketOnEEPROM_SuccessfullyIncreasesThePayloadLength()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t adress = 13;
    int32_t expectedPayloadLengthAdress = (
        adress +
        sizeof(packet.stx) +
        sizeof(packet.active)
    );
    int32_t expectedNewPayloadLength = (
        packet.payloadLength + sizeof(dataToAdd)
    );
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        EEPROMClass_put_param_idx_o2_v.at(0) == expectedPayloadLengthAdress && 
        EEPROMClass_put_param_t_o2_v.at(0) == expectedNewPayloadLength
    );
}

void JoinDataWithDataPacketOnEEPROM_SuccessfullyIncludesTheDataIntoTheCalculationOfTheCrc()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t adress = 13;
    int32_t expectedCRCAdress = (
        adress +
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength)
    );
    uint16_t resultingPayloadLength = static_cast<uint16_t>(sizeof(data) + sizeof(dataToAdd));
    uint8_t *resultingPayload = new uint8_t[resultingPayloadLength] {
        packet.payload[0],
        packet.payload[1],
        packet.payload[2],
        packet.payload[3],
        dataToAddPtr[0],
        dataToAddPtr[1],
        dataToAddPtr[2],
        dataToAddPtr[3],
    };
    uint32_t expectedCRC = CalculateCRC(resultingPayload, resultingPayloadLength);
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        EEPROMClass_put_param_idx_o3_v.at(0) == expectedCRCAdress && 
        EEPROMClass_put_param_t_o3_v.at(0) == expectedCRC
    );
    delete[](resultingPayload);
}

void JoinDataWithDataPacketOnEEPROM_WritesThePayloadInACircularPatternWhenExceedingTheEepromsLastAdress()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = static_cast<uint16_t>(
        SizeOfSerializedDataPacket(packet) + 
        sizeof(dataToAdd)
    );
    uint16_t adress = static_cast<uint16_t>(
        eepromSize - 
        (
            SizeOfSerializedDataPacket(packet) - 
            sizeof(packet.etx)
        ) - 
        (sizeof(dataToAdd)/2)
    );
    uint32_t expectedPayloadContinuationAdress = (
        adress +
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc) +
        packet.payloadLength
    );
    uint16_t expectedAddedDataAdress0 = CyclicAdress(expectedPayloadContinuationAdress + 0, eepromSize),
            expectedAddedDataAdress1 = CyclicAdress(expectedPayloadContinuationAdress + 1, eepromSize),
            expectedAddedDataAdress2 = CyclicAdress(expectedPayloadContinuationAdress + 2, eepromSize),
            expectedAddedDataAdress3 = CyclicAdress(expectedPayloadContinuationAdress + 3, eepromSize);
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        EEPROMClass_update_param_idx_v.at(0) == expectedAddedDataAdress0 && EEPROMClass_update_param_val_v.at(0) == dataToAddPtr[0] &&
        EEPROMClass_update_param_idx_v.at(1) == expectedAddedDataAdress1 && EEPROMClass_update_param_val_v.at(1) == dataToAddPtr[1] &&
        EEPROMClass_update_param_idx_v.at(2) == expectedAddedDataAdress2 && EEPROMClass_update_param_val_v.at(2) == dataToAddPtr[2] &&
        EEPROMClass_update_param_idx_v.at(3) == expectedAddedDataAdress3 && EEPROMClass_update_param_val_v.at(3) == dataToAddPtr[3]
    );
}

void JoinDataWithDataPacketOnEEPROM_StxIsAtTheLastPostitionOfTheEeprom_WritesEverythingInACircularPatternAtTheCorrectPlace()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = static_cast<uint16_t>(
        SizeOfSerializedDataPacket(packet) + 
        sizeof(dataToAdd)
    );
    uint16_t adress = static_cast<uint16_t>(
        eepromSize - 
        sizeof(packet.stx)
    );
    int32_t expectedPayloadLengthAdress = CyclicAdress((
        adress +
        sizeof(packet.stx) +
        sizeof(packet.active)
    ), eepromSize);
    int32_t expectedCRCAdress = CyclicAdress((
        expectedPayloadLengthAdress + 
        sizeof(packet.payloadLength)
    ), eepromSize);
    uint32_t expectedPayloadContinuationAdress = CyclicAdress((
        expectedCRCAdress + 
        sizeof(packet.crc) +
        packet.payloadLength
    ), eepromSize);
    uint16_t expectedAddedDataAdress0 = CyclicAdress(expectedPayloadContinuationAdress + 0, eepromSize),
            expectedAddedDataAdress1 = CyclicAdress(expectedPayloadContinuationAdress + 1, eepromSize),
            expectedAddedDataAdress2 = CyclicAdress(expectedPayloadContinuationAdress + 2, eepromSize),
            expectedAddedDataAdress3 = CyclicAdress(expectedPayloadContinuationAdress + 3, eepromSize);
    int32_t expectedNewEtxAdress = CyclicAdress((
        expectedAddedDataAdress3 +
        sizeof(dataToAddPtr[3]) 
    ), eepromSize);
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        EEPROMClass_update_param_idx_v.at(0) == expectedAddedDataAdress0 && EEPROMClass_update_param_val_v.at(0) == dataToAddPtr[0] &&
        EEPROMClass_update_param_idx_v.at(1) == expectedAddedDataAdress1 && EEPROMClass_update_param_val_v.at(1) == dataToAddPtr[1] &&
        EEPROMClass_update_param_idx_v.at(2) == expectedAddedDataAdress2 && EEPROMClass_update_param_val_v.at(2) == dataToAddPtr[2] &&
        EEPROMClass_update_param_idx_v.at(3) == expectedAddedDataAdress3 && EEPROMClass_update_param_val_v.at(3) == dataToAddPtr[3] &&
        EEPROMClass_put_param_idx_o2_v.at(0) == expectedPayloadLengthAdress && 
        EEPROMClass_put_param_idx_o3_v.at(0) == expectedCRCAdress && 
        EEPROMClass_update_param_idx_v.at(4) == expectedNewEtxAdress
    );
}

void JoinDataWithDataPacketOnEEPROM_JoinsDataWithPacket_ReturnsTrue()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = static_cast<uint16_t>(
        SizeOfSerializedDataPacket(packet) + 
        sizeof(dataToAdd)
    );
    uint16_t adress = static_cast<uint16_t>(
        eepromSize - 
        sizeof(packet.stx)
    );
    int32_t expectedPayloadLengthAdress = CyclicAdress((
        adress +
        sizeof(packet.stx) +
        sizeof(packet.active)
    ), eepromSize);
    int32_t expectedCRCAdress = CyclicAdress((
        expectedPayloadLengthAdress + 
        sizeof(packet.payloadLength)
    ), eepromSize);
    uint32_t expectedPayloadContinuationAdress = CyclicAdress((
        expectedCRCAdress + 
        sizeof(packet.crc) +
        packet.payloadLength
    ), eepromSize);
    uint16_t expectedAddedDataAdress0 = CyclicAdress(expectedPayloadContinuationAdress + 0, eepromSize),
            expectedAddedDataAdress1 = CyclicAdress(expectedPayloadContinuationAdress + 1, eepromSize),
            expectedAddedDataAdress2 = CyclicAdress(expectedPayloadContinuationAdress + 2, eepromSize),
            expectedAddedDataAdress3 = CyclicAdress(expectedPayloadContinuationAdress + 3, eepromSize);
    int32_t expectedNewEtxAdress = CyclicAdress((
        expectedAddedDataAdress3 +
        sizeof(dataToAddPtr[3]) 
    ), eepromSize);
    int32_t expectedNewPayloadLength = (
        sizeof(data) + sizeof(dataToAdd)
    );
    uint32_t expectedNewCRC = CalculateCRC(dataToAddPtr, sizeof(dataToAdd), packet.crc);
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        resultBool == true &&
        EEPROMClass_update_param_idx_v.at(0) == expectedAddedDataAdress0 && EEPROMClass_update_param_val_v.at(0) == dataToAddPtr[0] &&
        EEPROMClass_update_param_idx_v.at(1) == expectedAddedDataAdress1 && EEPROMClass_update_param_val_v.at(1) == dataToAddPtr[1] &&
        EEPROMClass_update_param_idx_v.at(2) == expectedAddedDataAdress2 && EEPROMClass_update_param_val_v.at(2) == dataToAddPtr[2] &&
        EEPROMClass_update_param_idx_v.at(3) == expectedAddedDataAdress3 && EEPROMClass_update_param_val_v.at(3) == dataToAddPtr[3] &&
        EEPROMClass_put_param_idx_o2_v.at(0) == expectedPayloadLengthAdress && EEPROMClass_put_param_t_o2_v.at(0) == expectedNewPayloadLength &&
        EEPROMClass_put_param_idx_o3_v.at(0) == expectedCRCAdress && EEPROMClass_put_param_t_o3_v.at(0) == expectedNewCRC &&
        EEPROMClass_update_param_idx_v.at(4) == expectedNewEtxAdress && EEPROMClass_update_param_val_v.at(4) == packet.etx
    );
}

void JoinDataWithDataPacketOnEEPROM_TheJoinedPacketWillBeBiggerThanTheEeprom_DoesNotWriteToEEPROMAndReturnsFalse()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t adress = 0;
    uint16_t eepromSize = static_cast<uint16_t>(
        SizeOfSerializedDataPacket(packet) + sizeof(dataToAdd) - 1
    );
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        resultBool == false &&
        EEPROMClass_update_invocations == 0 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0
    );
}

void JoinDataWithDataPacketOnEEPROM_AdressExceedsTheLastAdressOfTheEEPROM_ReturnsFalse()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = 100;
    uint16_t adress = eepromSize;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        resultBool == false &&
        EEPROMClass_update_invocations == 0 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0
    );
}

void JoinDataWithDataPacketOnEEPROM_AdressDoesNotPointToAnStx_ReturnsFalse()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t adress = 0;
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return_v.push_back(0);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(packet.etx);

    bool resultBool = JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        resultBool == false &&
        EEPROMClass_update_invocations == 0 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0
    );
}

void JoinDataWithDataPacketOnEEPROM_CanNotFindEtx_ReturnsFalse()
{
    uint32_t data = 1000;
    uint32_t dataToAdd = 123;
    uint8_t *dataToAddPtr = reinterpret_cast<uint8_t *>(&dataToAdd);
    DataPacket packet = DataToPacket(data);
    uint16_t adress = 0;
    EEPROMClass_length_return = 1024;
    EEPROMClass_read_return_v.push_back(packet.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(packet.crc);
    EEPROMClass_read_return_v.push_back(0);

    bool resultBool = JoinDataWithDataPacketOnEEPROM(adress, dataToAddPtr, sizeof(dataToAdd));

    ASSERT_TEST(
        resultBool == false &&
        EEPROMClass_update_invocations == 0 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0
    );
}


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
    uint16_t eepromSize = static_cast<uint16_t>(expectedPacket.payloadLength + 20);

    EEPROMClass_length_return_v.push_back(eepromSize);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(adress, expectedPacket, eepromSize);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_get_param_t_o2_vr.push_back(expectedPacket.crc);
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

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

    ASSERT_TEST(EEPROMClass_read_param_idx_v[0] == startAdress);
}

void FindFirstDataPacketOnEEPROM_FindsPacket_ReturnsTrue()
{
    uint16_t startAdress = 2;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(startAdress, expectedPacket, eepromSize);

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

    ASSERT_TEST(resultBool == true);
}

void FindFirstDataPacketOnEEPROM_PacketIsPresentAfterGarbageData_FindsPacket()
{
    uint16_t startAdress = 2;
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    uint16_t expectedPacketSize = SizeOfSerializedDataPacket(expectedPacket),
             expectedAdress = static_cast<uint16_t>(startAdress + 2);
    uint16_t expectedPayloadAdress = static_cast<uint16_t>(
        expectedAdress + 
        sizeof(expectedPacket.stx) +
        sizeof(expectedPacket.active) +
        sizeof(expectedPacket.payloadLength) +
        sizeof(expectedPacket.crc) 
    );
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(9);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress, expectedPacket, eepromSize);

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

    ASSERT_TEST(
        resultBool == true &&
        packetAdress == expectedAdress &&
        packetSize == expectedPacketSize && 
        payloadAdress == expectedPayloadAdress &&
        payloadLength == expectedPacket.payloadLength
    );
}

void FindFirstDataPacketOnEEPROM_PacketIsPresentAfterGarbageData_FindsPacketOnCorrectAdress()
{
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    uint16_t startAdress = 2;
    uint16_t expectedAdress = static_cast<uint16_t>(startAdress + 2);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return_v.push_back(13);
    EEPROMClass_read_return_v.push_back(9);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(expectedAdress, expectedPacket, eepromSize);

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

    ASSERT_TEST(
        resultBool == true &&
        packetAdress == expectedAdress
    );
}

void FindFirstDataPacketOnEEPROM_FindsPacket_ReturnsCorrectPacketSize()
{
    uint16_t startAdress = 2;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint8_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    uint16_t expectedPacketSize = Helper_CalculateSizeOfPacketOnEEPROM(expectedPacket);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(startAdress, expectedPacket, eepromSize);

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

    ASSERT_TEST(
        resultBool == true &&
        packetSize == expectedPacketSize
    );
}

void FindFirstDataPacketOnEEPROM_FindsPacket_ReturnsCorrectPayloadAdress()
{
    uint16_t startAdress = 2;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint8_t data = 22;
    DataPacket packet = DataToPacket(data);
    uint16_t expectedPayloadAdress = static_cast<uint16_t>(
        startAdress +
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc)
    );
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(startAdress, packet, eepromSize);

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

    ASSERT_TEST(
        resultBool == true &&
        payloadAdress == expectedPayloadAdress
    );
}

void FindFirstDataPacketOnEEPROM_FindsPacket_ReturnsCorrectPayloadLength()
{
    uint16_t startAdress = 2;
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint32_t data = 22;
    DataPacket expectedPacket = DataToPacket(data);
    Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(startAdress, expectedPacket, eepromSize);

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

    ASSERT_TEST(
        resultBool == true &&
        payloadLength == expectedPacket.payloadLength
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

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

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
    uint16_t eepromSize = 20;
    EEPROMClass_length_return = eepromSize;
    EEPROMClass_read_return = 0x00;

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

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

    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    bool resultBool = FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength);

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
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress, packet, eepromSize);
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
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress, packet, eepromSize);
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
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress, packet, eepromSize);
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
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress, packet, eepromSize);
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
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress1, packet1, eepromSize);
    EEPROMClass_read_return_v.push_back(packet1.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet1.payloadLength);
    EEPROMClass_read_return_v.push_back(packet1.etx);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress2, packet2, eepromSize);
    EEPROMClass_read_return_v.push_back(packet2.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(packet2.payloadLength);
    EEPROMClass_read_return_v.push_back(packet2.etx);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(expectedAdress3, packet3, eepromSize);
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

// void SaveDataPacketToEEPROM_PacketIsSavedButEepromFailsToReadTheData_ReturnsFalse()
// {
//     uint16_t data = 42;
//     uint8_t *dataPtr = reinterpret_cast<uint8_t*>(&data);
//     uint16_t adress = 20;
//     DataPacket packet;

//     // This ensure that SaveDataPacketToEEPROM and ReadDataPacketOnEEPROM knows the size of the eeprom.
//     EEPROMClass_length_return = sizeof(data) + 20;
//     // This ensures that ReadDataPacketOnEEPROM returns false
//     EEPROMClass_read_return_v.push_back(packet.stx);
//     EEPROMClass_read_return_v.push_back(packet.active);
//     EEPROMClass_get_param_t_o1_vr.push_back(sizeof(data));
//     EEPROMClass_get_param_t_o2_vr.push_back(55561893);
//     EEPROMClass_read_return_v.push_back(packet.etx);
//     EEPROMClass_read_return_v.push_back(static_cast<uint8_t>(dataPtr[0] + 13));
//     EEPROMClass_read_return_v.push_back(dataPtr[1]);

//     uint16_t packetSize;
//     bool resultBool = SaveDataPacketToEEPROM(adress, dataPtr, sizeof(data), &packetSize);

//     ASSERT_TEST(resultBool == false);
// }


void DataPacketWriter_Constructor_IsCompletedVariableIsSetToFalse()
{
    uint16_t address = 20;
    uint16_t eepromSize = 100;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(address);

    ASSERT_TEST(
        packetWriter.isCompleted == false
    );
}

void DataPacketWriter_Constructor_AddressVariableIsSetToTheGivenPacketAdress()
{
    uint16_t expectedAddress = 20;
    uint16_t eepromSize = 100;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(expectedAddress);

    ASSERT_TEST(
        packetWriter.address == expectedAddress
    );
}

void DataPacketWriter_Constructor_PayloadLengthVariableIsSetToZero()
{
    uint16_t address = 20;
    uint16_t eepromSize = 100;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(address);

    ASSERT_TEST(
        packetWriter.payloadLength == 0
    );
}

void DataPacketWriter_Constructor_SizeOfEepromVariableIsSetToTheSizeOfTheEEPROM()
{
    uint16_t address = 20;
    uint16_t expectedEepromSize = 100;
    EEPROMClass_length_return = expectedEepromSize;

    DataPacketWriter packetWriter(address);

    ASSERT_TEST(
        packetWriter.sizeOfEeprom == expectedEepromSize
    );
}

void DataPacketWriter_Constructor_WritesStxToGivenAdress()
{
    uint16_t address = 20;
    uint16_t eepromSize = 100;
    EEPROMClass_length_return = eepromSize;
    DataPacket packet;

    DataPacketWriter packetWriter(address);

    ASSERT_TEST(
        EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(address) &&
        EEPROMClass_put_param_t_o1_v[0] == packet.stx
    );
}

void DataPacketWriter_Constructor_WritesStxToGivenAdress_SuccessVariableIsSetToTrue()
{
    uint16_t address = 20;
    uint16_t eepromSize = 100;
    EEPROMClass_length_return = eepromSize;
    DataPacket packet;

    DataPacketWriter packetWriter(address);

    ASSERT_TEST(
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(address) &&
        EEPROMClass_put_param_t_o1_v[0] == packet.stx
    );
}

void DataPacketWriter_Constructor_WroteDownStx_AddsSizeOfStxToPacketSizeVariable()
{
    uint16_t address = 20;
    uint16_t eepromSize = 100;
    EEPROMClass_length_return = eepromSize;
    DataPacket packet;
    
    DataPacketWriter packetWriter(address);

    ASSERT_TEST(
        packetWriter.packetSize == sizeof(packet.stx)
    );
}

void DataPacketWriter_Constructor_AdressIsOutsideOfEEPROMsRange_DoesNotWriteAnythingToEEPROM()
{
    uint16_t eepromSize, address;
    eepromSize = address = 1024;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(address);

    ASSERT_TEST(
        EEPROMClass_put_invocations_o1 == 0 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0 &&
        EEPROMClass_update_invocations == 0
    );
}

void DataPacketWriter_Constructor_AdressIsOutsideOfEEPROMsRange_SuccessVariableIsSetToFalse()
{
    uint16_t eepromSize, address;
    eepromSize = address = 1024;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(address);

    ASSERT_TEST(packetWriter.success == false);
}

void DataPacketWriter_Constructor_PacketWillNotFitEEPROM_SuccessVariableIsSetToFalse()
{
    uint16_t eepromSize = static_cast<uint16_t>(
        SizeOfEmptySerializedDataPacket() - 1
    ); 
    uint16_t address = 0;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(address);

    ASSERT_TEST(packetWriter.success == false);
}

void AddDataToPayload_PacketIsTooBigForEEPROM_DoesNotWritePayloadToMemoryAndReturnsFalse()
{
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t packetSize = SizeOfSerializedDataPacket(packet),
                address = 0,
                eepromSize = static_cast<uint16_t>(packetSize - 1);
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(address);
    bool fitsPacketBeforeWeAddToPayload = packetWriter.success;
    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);

    ASSERT_TEST(
        fitsPacketBeforeWeAddToPayload == true &&
        resultBool == false &&
        packetWriter.success == false &&
        EEPROMClass_put_invocations_o1 == 1 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0 &&
        EEPROMClass_update_invocations == 0
    );
}

void AddDataToPayload_PacketBecomesTooBigForEEPROM_DoesNotWritePayloadToMemoryAndReturnsFalse()
{
    uint64_t data1 = 8409,
             data2 = 4321;
    DataPacket packet1 = DataToPacket(data1),
               packet2 = DataToPacket(data2);
    uint16_t packetSize1 = SizeOfSerializedDataPacket(packet1),
                eepromSize = static_cast<uint16_t>(packetSize1),
                address = 0;
    EEPROMClass_length_return = eepromSize;
    DataPacketWriter packetWriter(address);
    bool initialResultBool = packetWriter.AddDataToPayload(packet1.payload, packet1.payloadLength);
    uint32_t expectedUpdateInvocations = EEPROMClass_update_invocations;

    bool fitsPacketBeforeWeAddToPayload = packetWriter.success && initialResultBool;
    bool resultBool = packetWriter.AddDataToPayload(packet2.payload, packet2.payloadLength);

    ASSERT_TEST(
        fitsPacketBeforeWeAddToPayload == true &&
        resultBool == false &&
        packetWriter.success == false &&
        EEPROMClass_put_invocations_o1 == 1 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0 &&
        EEPROMClass_update_invocations == expectedUpdateInvocations
    );
}

void AddDataToPayload_AdaptsSizeOfPacketToFitData()
{
    uint16_t eepromSize = 1024;
    uint16_t data = 42;
    DataPacket packet = DataToPacket(data);
    uint16_t adress = 20;
    uint16_t packetSize;
    int expectedPayloadAdress = CyclicAdress(
        (
            adress +
            SizeOfEmptySerializedDataPacket()
            - sizeof(DataPacket::etx)
        )
        , eepromSize
    );
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(adress);
    packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);

    ASSERT_TEST(
        EEPROMClass_update_param_idx_v[0] == expectedPayloadAdress && EEPROMClass_update_param_val_v[0] == packet.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == expectedPayloadAdress + 1 && EEPROMClass_update_param_val_v[1] == packet.payload[1]
    );
}

void AddDataToPayload_PacketFitsOnEEPROM_ReturnsTrueAndSuccessIsTrue()
{
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t address = 0,
             eepromSize = SizeOfSerializedDataPacket(packet);
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(address);
    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);

    ASSERT_TEST(
        resultBool == true && 
        packetWriter.success == true
    );
}

void AddDataToPayload_SplitsPacketOnPayload_PutsDownPayloadOnCorrectAdresses()
{
    uint32_t data = 888;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = static_cast<uint16_t>(
        SizeOfSerializedDataPacket(packet) + 
        5
    );
    uint16_t relativePayloadAdress = sizeof(DataPacket::stx) + 
                                     sizeof(DataPacket::active) +
                                     sizeof(DataPacket::payloadLength) +
                                     sizeof(DataPacket::crc);
    uint16_t adress = static_cast<uint16_t>(
        eepromSize - (
            relativePayloadAdress +
            (sizeof(data) / 2)
        )
    );
    uint16_t expectedPayloadAdressPart0 = CyclicAdress((adress + relativePayloadAdress + 0), eepromSize),
             expectedPayloadAdressPart1 = CyclicAdress((adress + relativePayloadAdress + 1), eepromSize),
             expectedPayloadAdressPart2 = CyclicAdress((adress + relativePayloadAdress + 2), eepromSize),
             expectedPayloadAdressPart3 = CyclicAdress((adress + relativePayloadAdress + 3), eepromSize);
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(adress);
    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);

    ASSERT_TEST(
        resultBool == true &&
        EEPROMClass_update_param_idx_v[0] == static_cast<int>(expectedPayloadAdressPart0) && EEPROMClass_update_param_val_v[0] == packet.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == static_cast<int>(expectedPayloadAdressPart1) && EEPROMClass_update_param_val_v[1] == packet.payload[1] &&
        EEPROMClass_update_param_idx_v[2] == static_cast<int>(expectedPayloadAdressPart2) && EEPROMClass_update_param_val_v[2] == packet.payload[2] &&
        EEPROMClass_update_param_idx_v[3] == static_cast<int>(expectedPayloadAdressPart3) && EEPROMClass_update_param_val_v[3] == packet.payload[3]
    );
}



void AddDataToPayload_PayloadAndStxIsCorrectlyPutDown()
{
    uint16_t data = 42;
    uint16_t adress = 20;
    DataPacket packet = DataToPacket(data);
    unsigned int expectedStxAdress = static_cast<int>(adress), 
                 expectedActiveFlagAdress = expectedStxAdress + sizeof(DataPacket::stx), 
                 expectedPayloadLengthAdress = expectedActiveFlagAdress + sizeof(DataPacket::active), 
                 expectedCRCAdress = expectedPayloadLengthAdress + sizeof(DataPacket::payloadLength), 
                 expectedPayloadAdress = expectedCRCAdress + sizeof(DataPacket::crc), 
                 expectedEtxAdress = expectedPayloadAdress + sizeof(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(adress);
    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(expectedStxAdress) && EEPROMClass_put_param_t_o1_v[0] == packet.stx &&
        EEPROMClass_update_param_idx_v[0] == static_cast<int>(expectedPayloadAdress) && EEPROMClass_update_param_val_v[0] == packet.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == static_cast<int>(expectedPayloadAdress) + 1 && EEPROMClass_update_param_val_v[1] == packet.payload[1]
    );
}

void AddDataToPayload_AddsMultipleParts_PayloadAndStxIsCorrectlyPutDown()
{
    uint16_t data1 = 42;
    uint8_t data2 = 13,
            data3 = 37;
    uint16_t adress = 20;
    DataPacket packet1 = DataToPacket(data1),
               packet2 = DataToPacket(data2),
               packet3 = DataToPacket(data3);
    unsigned int expectedStxAdress = static_cast<int>(adress), 
                 expectedActiveFlagAdress = expectedStxAdress + sizeof(DataPacket::stx), 
                 expectedPayloadLengthAdress = expectedActiveFlagAdress + sizeof(DataPacket::active), 
                 expectedCRCAdress = expectedPayloadLengthAdress + sizeof(DataPacket::payloadLength), 
                 expectedPayloadAdress = expectedCRCAdress + sizeof(DataPacket::crc), 
                 expectedEtxAdress = expectedPayloadAdress + sizeof(data1) + sizeof(data2) + sizeof(data3);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(adress);
    bool resultBool1 = packetWriter.AddDataToPayload(packet1.payload, packet1.payloadLength);
    bool resultBool2 = packetWriter.AddDataToPayload(packet2.payload, packet2.payloadLength);
    bool resultBool3 = packetWriter.AddDataToPayload(packet3.payload, packet3.payloadLength);

    ASSERT_TEST(
        resultBool1 == true &&
        resultBool2 == true &&
        resultBool3 == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(expectedStxAdress) && EEPROMClass_put_param_t_o1_v[0] == packet1.stx &&
        EEPROMClass_update_param_idx_v[0] == static_cast<int>(expectedPayloadAdress + 0) && EEPROMClass_update_param_val_v[0] == packet1.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == static_cast<int>(expectedPayloadAdress + 1) && EEPROMClass_update_param_val_v[1] == packet1.payload[1] &&
        EEPROMClass_update_param_idx_v[2] == static_cast<int>(expectedPayloadAdress + 2) && EEPROMClass_update_param_val_v[2] == packet2.payload[0] &&
        EEPROMClass_update_param_idx_v[3] == static_cast<int>(expectedPayloadAdress + 3) && EEPROMClass_update_param_val_v[3] == packet3.payload[0]
    );
}


void AddDataToPayload_PacketIsAlreadyCompleted_ReturnsFalseAndDoesNotWriteToStorage()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t adress = 20;
    DataPacket packet = DataToPacket(data);
    DataPacketWriter packetWriter(adress);
    packetWriter.isCompleted = true;

    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);

    ASSERT_TEST(
        resultBool == false &&
        EEPROMClass_update_invocations == 0
    );
}

void AddDataToPayload_WroteDownData_AddsSizeOfWrittenDataToPayloadLengthVariable()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42,
             adress = 20;
    DataPacket packet = DataToPacket(data);
    int32_t expectedAddedPayloadLength = packet.payloadLength;
    DataPacketWriter packetWriter(adress);

    uint16_t initialPayloadLength = packetWriter.payloadLength;
    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);
    int32_t sizeOfAddedPayload = packetWriter.payloadLength - initialPayloadLength;

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        sizeOfAddedPayload == expectedAddedPayloadLength
    );
}

void AddDataToPayload_AddsMultipleParts_AddsSizeOfEachDataPartToPayloadLengthVariable()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data1 = 42,
             data2 = 451,
             adress = 20;
    DataPacket packet1 = DataToPacket(data1),
               packet2 = DataToPacket(data2);
    int32_t expectedAddedPayloadLength = (
        packet1.payloadLength +
        packet2.payloadLength
    );
    DataPacketWriter packetWriter(adress);

    uint16_t initialPayloadLength = packetWriter.payloadLength;
    packetWriter.AddDataToPayload(packet1.payload, packet1.payloadLength);
    bool resultBool = packetWriter.AddDataToPayload(packet2.payload, packet2.payloadLength);
    int32_t sizeOfAddedPayload = packetWriter.payloadLength - initialPayloadLength;

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        sizeOfAddedPayload == expectedAddedPayloadLength
    );
}

void AddDataToPayload_WroteDownData_AddsSizeOfWrittenDataToPacketSizeVariable()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42,
             adress = 20;
    DataPacket packet = DataToPacket(data);
    int32_t expectedAddedPacketSize = packet.payloadLength;
    DataPacketWriter packetWriter(adress);

    uint16_t initialPacketSize = packetWriter.packetSize;
    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);
    int32_t sizeOfAddedData = packetWriter.packetSize - initialPacketSize;

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        sizeOfAddedData == expectedAddedPacketSize
    );
}

void AddDataToPayload_AddsMultipleParts_AddsSizeOfEachDataPartToPacketSizeVariable()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data1 = 42,
             data2 = 437,
             adress = 20;
    DataPacket packet1 = DataToPacket(data1),
               packet2 = DataToPacket(data2);
    int32_t expectedAddedPacketSize = (
        packet1.payloadLength +
        packet2.payloadLength
    );
    DataPacketWriter packetWriter(adress);

    uint16_t initialPacketSize = packetWriter.packetSize;
    packetWriter.AddDataToPayload(packet1.payload, packet1.payloadLength);
    bool resultBool = packetWriter.AddDataToPayload(packet2.payload, packet2.payloadLength);
    int32_t sizeOfAddedData = packetWriter.packetSize - initialPacketSize;

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        sizeOfAddedData == expectedAddedPacketSize
    );
}

void AddDataToPayload_WroteDownData_CalculatesCrcOfPayload() 
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42,
             adress = 5;
    DataPacket packet = DataToPacket(data);
    uint32_t expectedCrc = CalculateCRC(packet.payload, packet.payloadLength);
    DataPacketWriter packetWriter(adress);

    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        packetWriter.crc == expectedCrc
    );
}

void AddDataToPayload_WroteDownMultipleDataParts_AddsEachPartToCrc()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint32_t data = 342;
    uint16_t adress = 5;
    DataPacket packet = DataToPacket(data);
    uint32_t expectedCrc = CalculateCRC((packet.payload + 0), sizeof(packet.payload[0]));
    expectedCrc = CalculateCRC((packet.payload + 1), sizeof(packet.payload[1]), expectedCrc);
    expectedCrc = CalculateCRC((packet.payload + 2), sizeof(packet.payload[2]), expectedCrc);
    expectedCrc = CalculateCRC((packet.payload + 3), sizeof(packet.payload[3]), expectedCrc);
    DataPacketWriter packetWriter(adress);

    packetWriter.AddDataToPayload((packet.payload + 0), sizeof(packet.payload[0]));
    packetWriter.AddDataToPayload((packet.payload + 1), sizeof(packet.payload[1]));
    packetWriter.AddDataToPayload((packet.payload + 2), sizeof(packet.payload[2]));
    bool resultBool = packetWriter.AddDataToPayload((packet.payload + 3), sizeof(packet.payload[3]));

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        packetWriter.crc == expectedCrc
    );
}

void AddDataToPayload_DataPacketWriterIsUnsuccessful_DoesNotWriteToStorageAndReturnsFalse()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t address = 0;
    DataPacketWriter packetWriter(address);
    packetWriter.success = false;

    bool resultBool = packetWriter.AddDataToPayload(packet.payload, packet.payloadLength);

    ASSERT_TEST(
        resultBool == false &&
        packetWriter.success == false &&
        EEPROMClass_put_invocations_o1 == 1 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0 &&
        EEPROMClass_update_invocations == 0
    );
}

void AddDataToPayload_TemplateFunction_PayloadAndStxIsCorrectlyPutDown()
{
    uint16_t data = 42;
    uint16_t adress = 20;
    DataPacket expectedPacket = DataToPacket(data);
    unsigned int expectedStxAdress = static_cast<int>(adress), 
                 expectedActiveFlagAdress = expectedStxAdress + sizeof(DataPacket::stx), 
                 expectedPayloadLengthAdress = expectedActiveFlagAdress + sizeof(DataPacket::active), 
                 expectedCRCAdress = expectedPayloadLengthAdress + sizeof(DataPacket::payloadLength), 
                 expectedPayloadAdress = expectedCRCAdress + sizeof(DataPacket::crc), 
                 expectedEtxAdress = expectedPayloadAdress + sizeof(data);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(adress);
    bool resultBool = packetWriter.AddDataToPayload(data);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(expectedStxAdress) && EEPROMClass_put_param_t_o1_v[0] == expectedPacket.stx &&
        EEPROMClass_update_param_idx_v[0] == static_cast<int>(expectedPayloadAdress) && EEPROMClass_update_param_val_v[0] == expectedPacket.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == static_cast<int>(expectedPayloadAdress) + 1 && EEPROMClass_update_param_val_v[1] == expectedPacket.payload[1]
    );
}

void AddDataToPayload_TemplateFunction_AddsMultipleParts_PayloadAndStxIsCorrectlyPutDown()
{
    uint16_t data1 = 42;
    uint8_t data2 = 13,
            data3 = 37;
    uint16_t adress = 20;
    DataPacket expectedPacket1 = DataToPacket(data1),
               expectedPacket2 = DataToPacket(data2),
               expectedPacket3 = DataToPacket(data3);
    unsigned int expectedStxAdress = static_cast<int>(adress), 
                 expectedActiveFlagAdress = expectedStxAdress + sizeof(DataPacket::stx), 
                 expectedPayloadLengthAdress = expectedActiveFlagAdress + sizeof(DataPacket::active), 
                 expectedCRCAdress = expectedPayloadLengthAdress + sizeof(DataPacket::payloadLength), 
                 expectedPayloadAdress = expectedCRCAdress + sizeof(DataPacket::crc), 
                 expectedEtxAdress = expectedPayloadAdress + sizeof(data1) + sizeof(data2) + sizeof(data3);
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;

    DataPacketWriter packetWriter(adress);
    bool resultBool1 = packetWriter.AddDataToPayload(data1);
    bool resultBool2 = packetWriter.AddDataToPayload(data2);
    bool resultBool3 = packetWriter.AddDataToPayload(data3);

    ASSERT_TEST(
        resultBool1 == true &&
        resultBool2 == true &&
        resultBool3 == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[0] == static_cast<int>(expectedStxAdress) && EEPROMClass_put_param_t_o1_v[0] == expectedPacket1.stx &&
        EEPROMClass_update_param_idx_v[0] == static_cast<int>(expectedPayloadAdress + 0) && EEPROMClass_update_param_val_v[0] == expectedPacket1.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == static_cast<int>(expectedPayloadAdress + 1) && EEPROMClass_update_param_val_v[1] == expectedPacket1.payload[1] &&
        EEPROMClass_update_param_idx_v[2] == static_cast<int>(expectedPayloadAdress + 2) && EEPROMClass_update_param_val_v[2] == expectedPacket2.payload[0] &&
        EEPROMClass_update_param_idx_v[3] == static_cast<int>(expectedPayloadAdress + 3) && EEPROMClass_update_param_val_v[3] == expectedPacket3.payload[0]
    );
}


void FinishWritingPacket_WritesActiveFlagToCorrectAddress()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = 20;
    DataPacket packet = DataToPacket(data);
    unsigned int expectedStxAdress = static_cast<int>(address), 
                 expectedActiveFlagAdress = expectedStxAdress + sizeof(DataPacket::stx);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[1] == static_cast<int>(expectedActiveFlagAdress) && 
        EEPROMClass_put_param_t_o1_v[1] == packet.active
    );
}

void FinishWritingPacket_ActiveFlagExceedsStorage_WritesActiveFlagAtTheStartOfTheStorage()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = static_cast<uint16_t>(
        eepromSize - sizeof(DataPacket::stx)
    );
    DataPacket packet = DataToPacket(data);
    int expectedStxAdress = CyclicAdress(address, eepromSize);
    int expectedActiveFlagAdress = CyclicAdress(
        expectedStxAdress + sizeof(DataPacket::stx)
        , eepromSize
    );
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[1] == expectedActiveFlagAdress && 
        EEPROMClass_put_param_t_o1_v[1] == packet.active
    );
}

void FinishWritingPacket_WritesPayloadLengthToCorrectAddress()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = 20;
    DataPacket packet = DataToPacket(data);
    int expectedStxAddress = static_cast<int>(address), 
        expectedActiveFlagAddress = expectedStxAddress + sizeof(DataPacket::stx), 
        expectedPayloadLengthAddress = expectedActiveFlagAddress + sizeof(DataPacket::active);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o2_v[0] == expectedPayloadLengthAddress &&
        EEPROMClass_put_param_t_o2_v[0] == sizeof(data)
    );
}

void FinishWritingPacket_PayloadLengthsAddressExceedsStorage_WritesPayloadLengthAtTheStartOfTheStorage()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = static_cast<uint16_t>(
        eepromSize
        - sizeof(DataPacket::stx)
        - sizeof(DataPacket::active)
    );
    DataPacket packet = DataToPacket(data);
    int expectedStxAddress = CyclicAdress(address, eepromSize),
        expectedActiveFlagAddress = CyclicAdress(
            expectedStxAddress + 
            sizeof(DataPacket::stx)
            , eepromSize
        ), 
        expectedPayloadLengthAddress = CyclicAdress(
            expectedActiveFlagAddress + 
            sizeof(DataPacket::active)
            , eepromSize
        );
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o2_v[0] == expectedPayloadLengthAddress &&
        EEPROMClass_put_param_t_o2_v[0] == sizeof(data)
    );
}

void FinishWritingPacket_WritesCrcToCorrectAddress()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = 20;
    DataPacket packet = DataToPacket(data);
    int expectedStxAddress = static_cast<int>(address), 
        expectedActiveFlagAddress = expectedStxAddress + sizeof(DataPacket::stx), 
        expectedPayloadLengthAddress = expectedActiveFlagAddress + sizeof(DataPacket::active),
        expectedCRCAddress = expectedPayloadLengthAddress + sizeof(DataPacket::payloadLength);
    uint32_t expectedCrc = CalculateCRC(packet.payload, packet.payloadLength);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o3_v[0] == expectedCRCAddress &&
        EEPROMClass_put_param_t_o3_v[0] == expectedCrc
    );
}

void FinishWritingPacket_CrcsAddressExceedsStorage_WritesCrcAtTheStartOfTheStorage()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = static_cast<uint16_t>(
        eepromSize
        - sizeof(DataPacket::stx)
        - sizeof(DataPacket::active)
        - sizeof(DataPacket::payloadLength)
    );    
    DataPacket packet = DataToPacket(data);
    int expectedStxAddress = CyclicAdress(address, eepromSize),
        expectedActiveFlagAddress = CyclicAdress(
            expectedStxAddress + 
            sizeof(DataPacket::stx)
            , eepromSize
        ), 
        expectedPayloadLengthAddress = CyclicAdress(
            expectedActiveFlagAddress + 
            sizeof(DataPacket::active)
            , eepromSize
        ),
        expectedCRCAddress = CyclicAdress(
            expectedPayloadLengthAddress + 
            sizeof(DataPacket::payloadLength)
            , eepromSize
        );
    uint32_t expectedCrc = CalculateCRC(packet.payload, packet.payloadLength);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o3_v[0] == expectedCRCAddress &&
        EEPROMClass_put_param_t_o3_v[0] == expectedCrc
    );
}

void FinishWritingPacket_WritesEtxToCorrectAddress()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = 20;
    DataPacket packet = DataToPacket(data);
    int expectedStxAddress = static_cast<int>(address), 
        expectedActiveFlagAddress = expectedStxAddress + sizeof(DataPacket::stx), 
        expectedPayloadLengthAddress = expectedActiveFlagAddress + sizeof(DataPacket::active),
        expectedCRCAddress = expectedPayloadLengthAddress + sizeof(DataPacket::payloadLength),
        expectedPayloadAddress = expectedCRCAddress + sizeof(DataPacket::crc), 
        expectedEtxAddress = expectedPayloadAddress + sizeof(data);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[2] == expectedEtxAddress &&
        EEPROMClass_put_param_t_o1_v[2] == packet.etx
    );
}

void FinishWritingPacket_EtxsAddressExceedsStorage_WritesEtxAtTheStartOfTheStorage()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = static_cast<uint16_t>(
        eepromSize
        - sizeof(DataPacket::stx)
        - sizeof(DataPacket::active)
        - sizeof(DataPacket::payloadLength)
        - sizeof(DataPacket::crc)
        - sizeof(data)
    );    
    DataPacket packet = DataToPacket(data);
    int expectedStxAddress = CyclicAdress(address, eepromSize),
        expectedActiveFlagAddress = CyclicAdress(
            expectedStxAddress + 
            sizeof(DataPacket::stx)
            , eepromSize
        ), 
        expectedPayloadLengthAddress = CyclicAdress(
            expectedActiveFlagAddress + 
            sizeof(DataPacket::active)
            , eepromSize
        ),
        expectedCRCAddress = CyclicAdress(
            expectedPayloadLengthAddress + 
            sizeof(DataPacket::payloadLength)
            , eepromSize
        ),
        expectedPayloadAddress = CyclicAdress(
            expectedCRCAddress +
            sizeof(DataPacket::crc)
            , eepromSize
        ),
        expectedEtxAddress = CyclicAdress(
            expectedPayloadAddress +
            sizeof(data)
            , eepromSize
        );
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[2] == expectedEtxAddress &&
        EEPROMClass_put_param_t_o1_v[2] == packet.etx
    );
}

void FinishWritingPacket_EtxIsPutDownAtTheEndOfThePacket()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint8_t data = 42;
    uint16_t address = 20;
    DataPacket packet = DataToPacket(data);
    int expectedEtxAddress = CyclicAdress(
        address +
        SizeOfSerializedDataPacket(packet)
        - sizeof(DataPacket::etx)
        , eepromSize
    );
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[2] == expectedEtxAddress &&
        EEPROMClass_put_param_t_o1_v[2] == packet.etx
    );
}

void FinishWritingPacket_PacketFitsOnEEPROM_ReturnsTrue()
{
    uint16_t address = 0;
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = SizeOfSerializedDataPacket(packet);
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true
    );
}

void FinishWritingPacket_Succeeds_ReturnsCorrectPacketSize()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t address = 0,
             expectedPacketSize = SizeOfSerializedDataPacket(packet);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);

    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);
    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        packetSize == expectedPacketSize
    );
}

void FinishWritingPacket_WritesMultipleDataPartsToPayloadAndSucceeds_ReturnsCorrectPacketSize()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint64_t data1 = 8409,
             data2 = 1234;
    uint16_t address = 0;
    uint16_t expectedPacketSize = static_cast<uint16_t>(
        SizeOfEmptySerializedDataPacket() +
        sizeof(data1) +
        sizeof(data2)
    );
    DataPacket packet1 = DataToPacket(data1),
               packet2 = DataToPacket(data2);
    uint16_t expectedPayloadLength = static_cast<uint16_t>(
        packet1.payloadLength + 
        packet2.payloadLength
    );
    uint8_t *expectedPayload = new uint8_t[expectedPayloadLength] {
        packet1.payload[0], packet1.payload[1], packet1.payload[2], packet1.payload[3],
        packet1.payload[4], packet1.payload[5], packet1.payload[6], packet1.payload[7],
        packet2.payload[0], packet2.payload[1], packet2.payload[2], packet2.payload[3],
        packet2.payload[4], packet2.payload[5], packet2.payload[6], packet2.payload[7],
    };
    DataPacket expectedPacket = DataPacket(expectedPayload, expectedPayloadLength);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, expectedPacket, eepromSize);

    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data1);
    packetWriter.AddDataToPayload(data2);
    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        packetSize == expectedPacketSize
    );
    delete[](expectedPayload);
}

void FinishWritingPacket_PacketWillExceedEndOfEEPROM_ReturnsCorrectPacketSize()
{
    uint32_t data = 888;
    DataPacket packet = DataToPacket(data);
    uint16_t expectedPacketSize = SizeOfSerializedDataPacket(packet),
            eepromSize = static_cast<uint16_t>(
                expectedPacketSize + 
                5
            ),
            address = static_cast<uint16_t>(
                eepromSize 
                - (expectedPacketSize / 2)
            );
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);

    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);
    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        packetSize == expectedPacketSize
    );
}

void FinishWritingPacket_PacketIsCorrectlyPutDown()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 42;
    uint16_t address = 20;
    DataPacket expectedPacket = DataToPacket(data);
    int expectedStxAddress = CyclicAdress(address, eepromSize), 
        expectedActiveFlagAddress = CyclicAdress(
            expectedStxAddress + 
            sizeof(DataPacket::stx)
            , eepromSize
        ), 
        expectedPayloadLengthAddress = CyclicAdress(
            expectedActiveFlagAddress + 
            sizeof(DataPacket::active)
            , eepromSize
        ), 
        expectedCRCAddress = CyclicAdress(
            expectedPayloadLengthAddress + 
            sizeof(DataPacket::payloadLength)
            , eepromSize
        ), 
        expectedPayloadAddress = CyclicAdress(
            expectedCRCAddress + 
            sizeof(DataPacket::crc)
            , eepromSize
        ), 
        expectedEtxAddress = CyclicAdress(
            expectedPayloadAddress + 
            sizeof(data)
            , eepromSize
        ), 
        expectedPacketSize = SizeOfSerializedDataPacket(expectedPacket);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, expectedPacket, eepromSize);

    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);
    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true && 
        packetWriter.success == true && 
        EEPROMClass_put_param_idx_o1_v[0] == expectedStxAddress && EEPROMClass_put_param_t_o1_v[0] == expectedPacket.stx &&
        EEPROMClass_put_param_idx_o1_v[1] == expectedActiveFlagAddress && EEPROMClass_put_param_t_o1_v[1] == expectedPacket.active &&
        EEPROMClass_put_param_idx_o2_v[0] == expectedPayloadLengthAddress && EEPROMClass_put_param_t_o2_v[0] == expectedPacket.payloadLength &&
        EEPROMClass_put_param_idx_o3_v[0] == expectedCRCAddress && EEPROMClass_put_param_t_o3_v[0] == CalculateCRC(expectedPacket.payload, expectedPacket.payloadLength) &&
        EEPROMClass_update_param_idx_v[0] == expectedPayloadAddress && EEPROMClass_update_param_val_v[0] == expectedPacket.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == expectedPayloadAddress + 1 && EEPROMClass_update_param_val_v[1] == expectedPacket.payload[1] &&
        EEPROMClass_put_param_idx_o1_v[2] == expectedEtxAddress && EEPROMClass_put_param_t_o1_v[2] == expectedPacket.etx &&
        packetSize == expectedPacketSize
    );
}

void FinishWritingPacket_WritesMultipleDataPartsToPayload_PacketIsCorrectlyPutDown()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data1 = 42;
    uint8_t data2 = 3;
    uint16_t address = 20;
    DataPacket templatePacket;
    DataPacket packet1 = DataToPacket(data1),
               packet2 = DataToPacket(data2);
    int expectedStxAddress = CyclicAdress(address, eepromSize), 
        expectedActiveFlagAddress = CyclicAdress(
            expectedStxAddress + 
            sizeof(DataPacket::stx)
            , eepromSize
        ), 
        expectedPayloadLengthAddress = CyclicAdress(
            expectedActiveFlagAddress + 
            sizeof(DataPacket::active)
            , eepromSize
        ), 
        expectedCRCAddress = CyclicAdress(
            expectedPayloadLengthAddress + 
            sizeof(DataPacket::payloadLength)
            , eepromSize
        ), 
        expectedPayloadAddress = CyclicAdress(
            expectedCRCAddress + 
            sizeof(DataPacket::crc)
            , eepromSize
        ), 
        expectedEtxAddress = CyclicAdress(
            expectedPayloadAddress + 
            sizeof(data1) +
            sizeof(data2)
            , eepromSize
        ), 
        expectedPacketSize = (
            SizeOfEmptySerializedDataPacket() +
            sizeof(data1) +
            sizeof(data2)
        ),
        expectedPayloadLength = (
            sizeof(data1) +
            sizeof(data2)
        );
    uint32_t expectedCrc = CalculateCRC(packet1.payload, packet1.payloadLength);
    expectedCrc = CalculateCRC(packet2.payload, packet2.payloadLength, expectedCrc);
    uint8_t *expectedPayload = new uint8_t[expectedPayloadLength] {
        packet1.payload[0], packet1.payload[1],
        packet2.payload[0],
    };
    DataPacket expectedPacket = DataPacket(expectedPayload, static_cast<uint16_t>(expectedPayloadLength));
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, expectedPacket, eepromSize);

    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data1);
    packetWriter.AddDataToPayload(data2);
    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true && 
        packetWriter.success == true && 
        EEPROMClass_put_param_idx_o1_v[0] == expectedStxAddress && EEPROMClass_put_param_t_o1_v[0] == templatePacket.stx &&
        EEPROMClass_put_param_idx_o1_v[1] == expectedActiveFlagAddress && EEPROMClass_put_param_t_o1_v[1] == templatePacket.active &&
        EEPROMClass_put_param_idx_o2_v[0] == expectedPayloadLengthAddress && EEPROMClass_put_param_t_o2_v[0] == expectedPayloadLength &&
        EEPROMClass_put_param_idx_o3_v[0] == expectedCRCAddress && EEPROMClass_put_param_t_o3_v[0] == expectedCrc &&
        EEPROMClass_update_param_idx_v[0] == expectedPayloadAddress + 0 && EEPROMClass_update_param_val_v[0] == packet1.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == expectedPayloadAddress + 1 && EEPROMClass_update_param_val_v[1] == packet1.payload[1] &&
        EEPROMClass_update_param_idx_v[2] == expectedPayloadAddress + 2 && EEPROMClass_update_param_val_v[2] == packet2.payload[0] &&
        EEPROMClass_put_param_idx_o1_v[2] == expectedEtxAddress && EEPROMClass_put_param_t_o1_v[2] == templatePacket.etx &&
        packetSize == expectedPacketSize
    );
    delete[](expectedPayload);
}

void FinishWritingPacket_PacketWillExceedEndOfEEPROM_SplitsPacketBetweenEndAndStartOfEEPROM()
{
    uint32_t data = 888;
    DataPacket packet = DataToPacket(data);
    uint16_t expectedPacketSize = SizeOfSerializedDataPacket(packet),
            eepromSize = static_cast<uint16_t>(
                expectedPacketSize + 5
            ),
            address = static_cast<uint16_t>(
                eepromSize 
                - (expectedPacketSize / 2)
            );
    uint16_t expectedStxAddress = CyclicAdress(address, eepromSize),
            expectedActiveFlagAddress = CyclicAdress(
                expectedStxAddress +
                sizeof(packet.stx)
                , eepromSize
            ),
            expectedPayloadLengthAddress = CyclicAdress(
                expectedActiveFlagAddress +
                sizeof(packet.active)
                , eepromSize
            ),
            expectedCRCAddress = CyclicAdress(
                expectedPayloadLengthAddress +
                sizeof(packet.payloadLength)
                , eepromSize
            ),
            expectedPayloadAddress = CyclicAdress(
                expectedCRCAddress +
                sizeof(packet.crc)
                , eepromSize
            ),
            expectedEtxAddress = CyclicAdress(
                expectedPayloadAddress +
                packet.payloadLength
                , eepromSize
            );
    uint16_t expectedPayloadAddressPart0 = CyclicAdress(expectedPayloadAddress + 0, eepromSize),
             expectedPayloadAddressPart1 = CyclicAdress(expectedPayloadAddress + 1, eepromSize),
             expectedPayloadAddressPart2 = CyclicAdress(expectedPayloadAddress + 2, eepromSize),
             expectedPayloadAddressPart3 = CyclicAdress(expectedPayloadAddress + 3, eepromSize);
    EEPROMClass_length_return = eepromSize;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);

    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);
    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        EEPROMClass_put_param_idx_o1_v[0] == expectedStxAddress && EEPROMClass_put_param_t_o1_v[0] == packet.stx &&
        EEPROMClass_put_param_idx_o1_v[1] == expectedActiveFlagAddress && EEPROMClass_put_param_t_o1_v[1] == packet.active &&
        EEPROMClass_put_param_idx_o2_v[0] == expectedPayloadLengthAddress && EEPROMClass_put_param_t_o2_v[0] == packet.payloadLength &&
        EEPROMClass_put_param_idx_o3_v[0] == expectedCRCAddress && EEPROMClass_put_param_t_o3_v[0] == packet.crc &&
        EEPROMClass_update_param_idx_v[0] == expectedPayloadAddressPart0 && EEPROMClass_update_param_val_v[0] == packet.payload[0] &&
        EEPROMClass_update_param_idx_v[1] == expectedPayloadAddressPart1 && EEPROMClass_update_param_val_v[1] == packet.payload[1] &&
        EEPROMClass_update_param_idx_v[2] == expectedPayloadAddressPart2 && EEPROMClass_update_param_val_v[2] == packet.payload[2] &&
        EEPROMClass_update_param_idx_v[3] == expectedPayloadAddressPart3 && EEPROMClass_update_param_val_v[3] == packet.payload[3] &&
        EEPROMClass_put_param_idx_o1_v[2] == expectedEtxAddress && EEPROMClass_put_param_t_o1_v[2] == packet.etx &&
        packetSize == expectedPacketSize
    );
}

void FinishWritingPacket_Succeeds_IsCompletedIsSetToTrue()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t address = 0;
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);

    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);
    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == true &&
        packetWriter.success == true &&
        packetWriter.isCompleted == true
    );
}

void FinishWritingPacket_SuccessIsFalse_ReturnsFalseAndDoesNotWriteToStorage()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t address = 0;
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);
    packetWriter.success = false;

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == false &&
        packetWriter.success == false &&
        EEPROMClass_put_invocations_o1 == 1 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0
    );
}

void FinishWritingPacket_PacketIsAlreadyCompleted_ReturnsFalseAndDoesNotWriteToStorage()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t address = 0;
    uint16_t data = 3617;
    DataPacket packet = DataToPacket(data);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, eepromSize);
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);
    packetWriter.isCompleted = true;

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == false &&
        packetWriter.isCompleted == true &&
        EEPROMClass_put_invocations_o1 == 1 &&
        EEPROMClass_put_invocations_o2 == 0 &&
        EEPROMClass_put_invocations_o3 == 0
    );
}

void FinishWritingPacket_PacketCannotBeReadFromStorage_SuccessIsFalseAndReturnsFalse()
{
    uint16_t eepromSize = 1024;
    EEPROMClass_length_return = eepromSize;
    uint16_t data = 3309;
    uint16_t address = 0;
    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);

    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == false &&
        packetWriter.success == false
    );
}

void DataPacketWriter_AllSteps_PacketDoesNotFitOnEEPROM_SuccessIsFalseAndReturnsFalse()
{
    uint16_t address = 0;
    uint64_t data = 8409;
    DataPacket packet = DataToPacket(data);
    uint16_t eepromSize = static_cast<uint16_t>(
        SizeOfSerializedDataPacket(packet) 
        - 1
    );
    EEPROMClass_length_return_v.push_back(eepromSize);
    Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(address, packet, 1024);

    DataPacketWriter packetWriter(address);
    packetWriter.AddDataToPayload(data);
    uint16_t packetSize = 0;
    bool resultBool = packetWriter.FinishWritingPacket(&packetSize);

    ASSERT_TEST(
        resultBool == false &&
        packetWriter.success == false
    );
}

// void FindFirstDataPacketOnEEPROM_TwoPacketsArePresent_StartAdressIsPutInBetweenPackages_FindsSecondPacketFirst(); // Note: can't be tested without somehow linking the idx to the output of the mocked function.
// void SaveDataPacketToEEPROM_PacketWillExceedEndOfEEPROM_SplitsPacketOnDataTypeBiggerThan1Byte_SuccessfullySplitsPacketWithoutLoosingData(); // Note: Can't be tested. Would test the eeprom library. 
