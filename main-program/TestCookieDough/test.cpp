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
    for(test_result result : results) {
        if(!result.passed) {
            printf("Test failed:\n");
            printf("Code: ");
            printf(result.code);
            printf("\n");
            printf("file: ");
            printf(result.file);
            printf("\n");
            printf("Line: ");
            printf((char*) result.line);

            failedTests++;
        }
    }

    int successfulTests = currentTestIndex - failedTests;
    printf("\n");
    printf((char*) successfulTests);
    printf(" out of ");
    printf((char*) currentTestIndex);
    printf(" tests passed.");
}

void Test(bool eval, char* code, const char* file, unsigned long line) 
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
    printf("-------------------\n");
    RunTests();
    printf("-------------------\n");
    PrintResults();
    printf("-------------------\n");
}

void RunTests() 
{
    printf("Running tests...\n");
    AddTwoNumbers_SumIsCorrect();
}