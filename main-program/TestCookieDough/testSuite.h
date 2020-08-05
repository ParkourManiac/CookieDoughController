#ifndef TEST_SUITE_H 
#define TEST_SUITE_H

/**
 * @brief A container for all tests to be run.
 * 
 * NOTE: Write your tests here! USE MACRO 'RUN_TEST(NameOfFunction)'. 
 * Example 'RUN_TEST(MyFunction)' will run the function 'void MyFunction();' as a test.
 * 
 * TESTS: When writing a new test make sure to use the 'ASSERT_TEST(condition)' macro to properly run the test. 
 * Example 'void ExampleTest() { ASSERT_TEST((2+2) == 4); }'.
 */
void RunTests();

/** // TODO: CHECK IF DESCRIPTION EXAMPLE IS CORRECT!
 * @brief A container for resetting all mock data between tests.
 * 
 * NOTE: Reset your mock data here!
 * Example: For the mock function...
 * ```
 * int sum_parameter_a;
 * int sum_parameter_b;
 * int sum_return_value;
 * unsigned int sum_invocation_count;
 * int sum(int a, int b) 
 * {
 *      sum_parameter_a = a;
 *      sum_parameter_b = b;
 *      return sum_return_value;
 * }
 * ```
 * 
 * write the following:
 * 
 * ```
 * void ResetMocks() 
 * {
 *      sum_parameter_a = int();
 *      sum_parameter_b = int();
 *      sum_return_value = int();
 *      sum_invocation_count = unsigned int();
 * }
 * ```
 * 
 */
void ResetMocks();


#endif
