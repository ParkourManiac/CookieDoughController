#include <stdio.h>
#include <string.h>

#define ASSERT_TEST(condition) Test(condition, #condition, __FILE__, __LINE__)
#define RUN_TEST(name) \
printf("\033[0;33m"); \
printf(#name); \
printf("\033[0m" "\n"); \
name();

void RunTests();
void PrintResults();
void Test(bool eval, const char* code, const char* file, unsigned long line);