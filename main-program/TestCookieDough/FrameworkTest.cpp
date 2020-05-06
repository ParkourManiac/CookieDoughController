#include "test.h"

void AddTwoNumbers_SumIsCorrect() {
    int num1 = 5;
    int num2 = 3;
    int expected = 8;

    int result = num1 + num2;

    ASSERT_TEST(result == expected);
}

void AddTwoNumbers_TestFails() {
    int num1 = 3;
    int num2 = 5;
    int expected = 8;

    int result = num1 + num2;

    ASSERT_TEST(result == expected);
}