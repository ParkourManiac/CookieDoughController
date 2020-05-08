#include "testSuite.h"
#include "test.h"

void GetItemUsingSubscriptOperator_GetsItem();
void AddItemToList_ListContainsItem();
void AddOneItemToList_ItemBecomesHead();
void AddOneItemToList_ItemBecomesTail();
void AddTwoItemsToList_FirstIsHeadSecondIsTail();
void AddMultipleItemsToList_FirstlyAddedIsHeadLastlyAddedIsTail();

/**
 * @brief A container for all tests to be run.
 * 
 * NOTE: Write your tests here! USE MACRO 'RUN_TEST(NameOfFunction)'. 
 * Example 'RUN_TEST(MyFunction)' will run the function 'void MyFunction();' as a test.
 * 
 * TESTS: When writing a new test make sure to use the 'ASSERT_TEST(condition)' macro to properly run the test. 
 * Example: 'void MyExampleTest() { ASSERT_TEST((2+2) == 4); }'.
 */
void RunTests() 
{
    //RUN_TEST(MyExampleTest);
    RUN_TEST(GetItemUsingSubscriptOperator_GetsItem);
    RUN_TEST(AddItemToList_ListContainsItem);
    RUN_TEST(AddOneItemToList_ItemBecomesHead);
    RUN_TEST(AddOneItemToList_ItemBecomesTail);
    RUN_TEST(AddTwoItemsToList_FirstIsHeadSecondIsTail);
    RUN_TEST(AddMultipleItemsToList_FirstlyAddedIsHeadLastlyAddedIsTail);
}