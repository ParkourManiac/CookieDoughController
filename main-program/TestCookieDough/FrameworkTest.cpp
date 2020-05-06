#include "test.h"

void AddTwoNumbers_SumIsCorrect() {
    int num1 = 5;
    int num2 = 3;
    int expected = 8;

    int result = num1 + num2;

    ASSERT_TEST(result == expected);
}