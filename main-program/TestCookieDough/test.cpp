#include "test.h"
#include "testSuite.h"

/**
 * @brief Contains the result of each test that has been run.
 */
test_result results[AMOUNT_OF_TESTS];
int currentTestIndex = 0;

void PrintResults()
{
    std::cout << ("Results: \n");
    int failedTests = 0;
    for (int i = 0; i < currentTestIndex; i++)
    {
        if (!results[i].passed)
        {
            std::cout <<("\033[1;31m"
                   "Test failed: ");
            std::cout <<("\033[0m"
                   "In file (");
            std::cout <<(results[i].file);
            std::cout <<(":");
            std::cout <<(results[i].line);
            std::cout <<(") while executing code snippet (");
            std::cout <<(results[i].code);
            std::cout <<(")\n");

            failedTests++;
        }
    }

    int successfulTests = currentTestIndex - failedTests;
    std::cout <<("\n");
    std::cout <<(successfulTests);
    std::cout <<(" out of ");
    std::cout <<(currentTestIndex);
    std::cout <<(" tests passed.\n");
    if (currentTestIndex == 0)
    {
        std::cout <<("\033[1;35m"
               "It's pretty empty over here... Writing a test in the testSuite.cpp file would fix that problem! ;D\n\n");
        std::cout <<("\033[0m");
    }
    else if (successfulTests == currentTestIndex)
    {
        std::cout <<("\033[1;32m"
               "All tests PASSED!\n\n");
        std::cout <<("\033[0m");
    }
    else
    {
        std::cout <<("\033[01;33m"
               "One or more tests FAILED!\n\n");
        std::cout <<("\033[0m");
    }
}

void Test(bool eval, const char *code, const char *file, unsigned long line)
{
    if (currentTestIndex >= AMOUNT_OF_TESTS)
    {
        std::cout <<("\033[01;31m"
               "\n\nThe amount of written tests have exceeded the limit of maximum allowed tests! Please increase the size of AMOUNT_OF_TESTS in test.h to fit more tests! \n");
        std::cout <<("Skipping test: (");
        std::cout <<(code);
        std::cout <<(") at line (");
        std::cout <<(line);
        std::cout <<(")");
        std::cout <<(") in file (");
        std::cout <<(file);
        std::cout <<(")\n\n");
        std::cout <<("\033[0m");
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
    std::cout <<("\n-------------------\n");
    std::cout <<("Running tests:\n");
    RunTests();
    PrintResults();
}
