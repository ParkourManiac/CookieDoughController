# USAGE
To use the CookieDoughTest framwork please do the following (there is an example of the testSuite.cpp file on the bottom of this page):
1. Create a file called testSuite.cpp inside the TestCookieDough directory.
2. Add the following to your testSuite.cpp file.
   1. `#include "testSuite.h"` 
   2. `#include "test.h"` 
   3. Lastly define the function `void RunTests() {}` at the bottom of the file.
3. Write your test as a function and assert the test condition using the `ASSERT_TEST(condition)` macro. See ExampleTest.cpp for an example. Note: Don't forget to include the test.h file in all your files that use the framework (`#include "test.h"`).
4. To run your test please populate the function body of `void RunTests() {}` with your test functions using the macro `RUN_TEST(functionName)`. See example below.

An example of the testSuite.cpp file could look like the following:
```
#include "testSuite.h"
#include "test.h"

void ExampleTest() 
{ 
    ASSERT_TEST((2+2) == 4); 
}

/**
 * @brief A container for all tests to be run.
 * 
 * NOTE: Write your tests here! USE MACRO 'RUN_TEST(NameOfFunction)'. 
 * Example 'RUN_TEST(MyFunction)' will run the function 'void MyFunction();' as a test.
 * 
 * TESTS: When writing a new test make sure to use the 'ASSERT_TEST(condition)' macro to properly run the test. 
 * Example: 'void ExampleTest() { ASSERT_TEST((2+2) == 4); }'.
 */
void RunTests() 
{
    RUN_TEST(ExampleTest);
}
```


# AUTO GENERATE "testSuite.cpp"
1. Create a file ending with "Test.cpp" (example: "ExampleTest.cpp"), next to the generateTestSuite.py file. (NOTE: This is the way the framework finds all files containing tests. It will take ALL functions defined as "void" inside all files ending with "Test.cpp" and mark each function as a test.)
2. Inside your file ending with "Test.cpp", include "test.h", then write a test using the 'ASSERT_TEST(condition)' macro.
3. Lastly run the "generateTestSuite.py" script using the "python" command and specify the directory in which the "testSuite.cpp" should be generated. (Example: "python generateTestSuite.py .", this will create a new testSuite.cpp file inside your current directory).