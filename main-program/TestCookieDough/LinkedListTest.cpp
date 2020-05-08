#include "test.h"
#include "../InDevelopment/LinkedList.cpp"

void GetFirstItemUsingSubscriptOperator_GetsItem()
{
    LinkedList<int> list;
    int expected = 1337;
    list.Add(expected);

    int result = *list[0];

    ASSERT_TEST(result == expected);
}

void GetSecondItemUsingSubscriptOperator_GetsSecondItem()
{
    LinkedList<int> list;
    int expected = 1337;
    list.Add(1);
    list.Add(expected);
}

void AddItemToList_ListContainsItem()
{
    LinkedList<int> list;
    int expected = 1337;

    list.Add(expected);

    ASSERT_TEST(*list[0] == expected);
}

void AddOneItemToList_ItemBecomesHead()
{
    LinkedList<int> list;
    int expected = 2;
    list.Add(expected);

    int result = list.head->value;

    ASSERT_TEST(result == expected);
}

void AddOneItemToList_ItemBecomesTail()
{
    LinkedList<int> list;
    int expected = 2;
    list.Add(expected);

    int result = list.tail->value;

    ASSERT_TEST(result == expected);
}

void AddTwoItemsToList_FirstIsHeadSecondIsTail()
{
    LinkedList<int> list;
    int expectedHead = 42;
    int expectedTail = 1337;

    list.Add(expectedHead);
    list.Add(expectedTail);
    int head = list.head->value;
    int tail = list.tail->value;

    ASSERT_TEST(head == expectedHead && tail == expectedTail);
}

void AddMultipleItemsToList_FirstlyAddedIsHeadLastlyAddedIsTail()
{
    LinkedList<int> list;
    int expectedHead = 42;
    int expectedTail = 1337;

    list.Add(expectedHead);
    list.Add(1);
    list.Add(2);
    list.Add(3);
    list.Add(4);
    list.Add(5);
    list.Add(expectedTail);
    int head = list.head->value;
    int tail = list.tail->value;

    ASSERT_TEST(head == expectedHead && tail == expectedTail);
}

//void AddMultipleItemsToList