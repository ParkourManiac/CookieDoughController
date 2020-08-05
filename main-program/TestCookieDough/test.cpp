#include "test.h"
#include "testSuite.h"

/**
 * @brief Contains the result of each test that has been run.
 */
test_result results[AMOUNT_OF_TESTS];
int currentTestIndex = 0;

void PrintResults()
{
    printf("Results: \n");
    int failedTests = 0;
    for (int i = 0; i < currentTestIndex; i++)
    {
        if (!results[i].passed)
        {
            printf("\033[1;31m"
                   "Test failed: ");
            printf("\033[0m"
                   "In file (");
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
    if (currentTestIndex == 0)
    {
        printf("\033[1;35m"
               "It's pretty empty over here... Writing a test in the testSuite.cpp file would fix that problem! ;D\n\n");
        printf("\033[0m");
    }
    else if (successfulTests == currentTestIndex)
    {
        printf("\033[1;32m"
               "All tests PASSED!\n\n");
        printf("\033[0m");
    }
    else
    {
        printf("\033[01;33m"
               "One or more tests FAILED!\n\n");
        printf("\033[0m");
    }
}

void Test(bool eval, const char *code, const char *file, unsigned long line)
{
    if (currentTestIndex >= AMOUNT_OF_TESTS)
    {
        printf("\033[01;31m"
               "\n\nThe amount of written tests have exceeded the limit of maximum allowed tests! Please increase the size of AMOUNT_OF_TESTS in test.h to fit more tests! \n");
        printf("Skipping test: (");
        printf(code);
        printf(") at line (");
        printf("%d", line);
        printf(")");
        printf(") in file (");
        printf(file);
        printf(")\n\n");
        printf("\033[0m");
        return;
    }

    test_result newResult;

    newResult.passed = eval;
    newResult.code = code;
    newResult.file = file;
    newResult.line = line;

    results[currentTestIndex] = newResult;
    currentTestIndex++;
}

void ResetMockData()
{
    ResetMocks();
}

void SetupColors()
{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // References:
    //SetConsoleMode() and ENABLE_VIRTUAL_TERMINAL_PROCESSING?
    //https://stackoverflow.com/questions/38772468/setconsolemode-and-enable-virtual-terminal-processing

    // Windows console with ANSI colors handling
    // https://superuser.com/questions/413073/windows-console-with-ansi-colors-handling
#endif
}

/**
 * @brief Handles running the tests and printing the result. 
 */
int main()
{
    SetupColors();
    printf("\n-------------------\n");
    printf("Running tests:\n");
    RunTests();
    PrintResults();
}
