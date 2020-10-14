#include "test.h"
#include "Fakes/Arduino.h"
#include "Fakes/EEPROM.h"

extern std::vector<uint8_t> EEPROMClass_read_return_v;
extern std::vector<int> EEPROMClass_read_param_idx_v;
extern std::vector<uint32_t > EEPROMClass_get_param_t_o2_vr;
extern uint32_t  EEPROMClass_get_param_t_o2_r;


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
    int first = EEPROMClass_read_param_idx_v[0];
    int second = EEPROMClass_read_param_idx_v[1];
    int third = EEPROMClass_read_param_idx_v[2];
    int fourth = EEPROMClass_read_param_idx_v[3];

    ASSERT_TEST(first == 1 && second == 2 && third == 3 && fourth == 4);
}

void TestIfOverwriteReferenceVectorIsWorking_Works()
{
    uint32_t expected1 = 1337;
    uint32_t expected2 = 13;
    EEPROMClass_get_param_t_o2_vr.push_back(expected1);
    EEPROMClass_get_param_t_o2_vr.push_back(expected2);
    uint32_t result1;
    uint32_t result2;

    EEPROM.get(0, result1);
    EEPROM.get(0, result2);

    ASSERT_TEST(result1 == expected1 && 
                result2 == expected2);
}

void TestIfOverwriteReferenceVectorResettingIsWorking_ResetsBetweenTests()
{
    uint32_t result1;

    EEPROM.get(0, result1);

    ASSERT_TEST(result1 != 1337);
}

void TestIfOverwriteReferenceIsWorking_Works()
{
    uint32_t expected = 1337;
    EEPROMClass_get_param_t_o2_r = expected;
    uint32_t result1;
    uint32_t result2;

    EEPROM.get(0, result1);
    EEPROM.get(0, result2);

    ASSERT_TEST(result1 == expected &&
                result2 == expected);
}

void TestIfOverwriteReferenceResettingIsWorking_ResetsBetweenTests()
{
    uint32_t result1;

    EEPROM.get(0, result1);

    ASSERT_TEST(result1 != 1337);
}