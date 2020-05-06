#include <stdio.h>
#include <string.h>

#define ASSERT_TEST(condition) Test(condition, #condition, __FILE__, __LINE__)

void RunTests();
void PrintResults();
void Test(bool eval, char* code, const char* file, unsigned long line);