#include "test.h"
/**
 * @brief This is an example of how you could write your tests. 
 * This test will succeed.
 */
void AddTwoNumbers_SumIsCorrect() {
    int num1 = 5;
    int num2 = 3;
    int expected = 8;

    int result = num1 + num2;

    ASSERT_TEST(result == expected);
}
/**
 * @brief This is an example of how you could write your tests. 
 * This test will fail since the result won't be equal to the expected value.
 */
void AddTwoNumbers_TestFails() {
    int num1 = 3;
    int num2 = 509123;
    int expected = 8;

    int result = num1 + num2;

    ASSERT_TEST(result == expected);
}
