#include <stdio.h>
#include <string.h>

/**
 * @brief Tests if a condition is successful.
 */
#define ASSERT_TEST(condition) Test(condition, #condition, __FILE__, __LINE__)
/**
 * @brief Runs the function provided as a test.
 */
#define RUN_TEST(functionName) \
printf("\033[0;33m"); \
printf(#functionName); \
printf("\033[0m" "\n"); \
functionName();

void RunTests();
void PrintResults();
void Test(bool eval, const char* code, const char* file, unsigned long line);