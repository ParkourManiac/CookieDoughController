#include "test.h"
#include "testSuite.h"

/**
 * @brief Stores useful information about a test result.
 */
struct test_result 
{
    bool passed;
    unsigned long line;
    const char* file;
    const char* code;
};

/**
 * @brief The max amount of test we can write.
 * Note: Just increase this value if more tests are needed.
 */
#define AMOUNT_OF_TESTS 100
int currentTestIndex = 0;
/**
 * @brief Contains the result of each test that has been run.
 */
test_result results[AMOUNT_OF_TESTS];

/**
 * @brief Prints the result of all tests and provides information about failed tests.
 */
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
    if(currentTestIndex == 0) {
        printf("\033[1;35m""It's pretty empty over here... Writing a test in the testSuite.cpp file would fix that problem! ;D\n\n");
        printf("\033[0m");
    } else if(successfulTests == currentTestIndex) {
        printf("\033[1;32m""All tests PASSED!\n\n");
        printf("\033[0m");
    } else {
        printf("\033[01;33m" "One or more tests FAILED!\n\n");
        printf("\033[0m");
    }
}

/**
 * @brief Used to convert a test evalutation into a test result.
 * WARNING: THIS IS NOT TO BE USED. USE MACRO 'ASSERT_TEST(condition)' INSTEAD.
 * 
 * @param eval The condition to be tested.
 * @param code The code of the evaluation.
 * @param file The file in which the test is defined.
 * @param line The line number from where this function was called.
 */
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

/**
 * @brief Handles running the tests and printing the result. 
 */
int main() {
    printf("\n-------------------\n");
    printf("Running tests:\n");
    RunTests();
    PrintResults();
}

// IMPORTANT: To run a test please run your function in the file testSuite.cpp RunTests() function using the macro RUN_TEST(functionName).