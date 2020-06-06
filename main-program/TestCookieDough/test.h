#ifndef TEST_H
#define TEST_H

#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
#include <windows.h>
#endif

// IMPORTANT: To run a test please run your function in the file
// testSuite.cpp RunTests() function using the macro RUN_TEST(functionName).
#include <stdio.h>

/**
 * @brief Stores useful information about a test result.
 */
struct test_result
{
    bool passed;
    unsigned long line;
    const char *file;
    const char *code;
};

/**
 * @brief Prints the result of all tests and provides information about failed tests.
 */
void PrintResults();

/**
 * @brief Used to convert a test evalutation into a test result.
 * WARNING: THIS IS NOT TO BE USED. USE MACRO 'ASSERT_TEST(condition)' INSTEAD.
 * 
 * @param eval The condition to be tested.
 * @param code The code of the evaluation.
 * @param file The file in which the test is defined.
 * @param line The line number from where this function was called.
 */
void Test(bool eval, const char *code, const char *file, unsigned long line);

/**
 * @brief Resets all data related to mocked functions. 
 */
void ResetMockData();

/**
 * @brief Enables the virtual terminal processing for windows so that the window can display colors.
 */
void SetupColors();

/**
 * @brief The max amount of test we can write.
 * Note: Just increase this value if more tests are needed.
 */
#define AMOUNT_OF_TESTS 100

/**
 * @brief Tests if a condition is successful.
 * Example: 'void MyFunction() { ASSERT_TEST((2+2) == 4); }'
 */
#define ASSERT_TEST(condition) Test(condition, #condition, __FILE__, __LINE__)
/**
 * @brief Runs the function provided as a test.
 * Example: 'RUN_TEST(MyFunction);'
 */
#define RUN_TEST(functionName) \
    printf("\033[1;33m");      \
    printf(#functionName);     \
    printf("\033[0m"           \
           "\n");              \
    functionName();            \
    ResetMockData();

#endif
