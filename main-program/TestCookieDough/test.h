#include <stdio.h>
#include <string.h>

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
printf("\033[0;33m"); \
printf(#functionName); \
printf("\033[0m" "\n"); \
functionName();

void RunTests();
void PrintResults();
void Test(bool eval, const char* code, const char* file, unsigned long line);
