#include "test.h"
#include "FrameworkTest.cpp"

struct test_result 
{
    bool passed;
    unsigned long line;
    const char* file;
    const char* code;
};

#define AMOUNT_OF_TESTS 100
int currentTestIndex = 0;
test_result results[AMOUNT_OF_TESTS];

void PrintResults() 
{
    printf("Results: \n");
    int failedTests = 0;
    for(int i = 0; i < currentTestIndex; i++) {
        if(!results[i].passed) {
            printf("\033[1;31m" "Test failed: ");
            printf("\033[0m" "In file (");
            printf("%s", results[i].file);
            printf(") at line (");   
            printf("%lu", results[i].line);
            printf(") while executing code snippet (");
            printf("%s", results[i].code);
            printf(")\n");

            failedTests++;
        }
    }

    int successfulTests = currentTestIndex - failedTests;
    printf("\n");
    printf("%d", successfulTests);
    printf(" out of ");
    printf("%d", currentTestIndex);
    printf(" tests passed.\n");
    if(successfulTests == currentTestIndex) {
        printf("\033[1;32m""All tests PASSED!\n\n");
        printf("\033[0m");
    } else {
        printf("\033[01;33m" "One or more tests FAILED!\n\n");
        printf("\033[0m");
    }
}

void Test(bool eval, const char* code, const char* file, unsigned long line) 
{
    test_result newResult;

    newResult.passed = eval;
    newResult.code = code;
    newResult.file = file;
    newResult.line = line;

    results[currentTestIndex] = newResult;
    currentTestIndex++;
}

int main() {
    printf("\n-------------------\n");
    RunTests();
    PrintResults();
}

void RunTests() 
{
    printf("Running tests:\n");
    RUN_TEST(AddTwoNumbers_SumIsCorrect);
    RUN_TEST(AddTwoNumbers_TestFails);
}