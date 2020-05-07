#include "test.h"
#include "../InDevelopment/LinkedList.cpp"

void AddItemToList_ListContainsItem() {
    LinkedList<int> list;
    int expected = 1;

    list.Add(expected);

    ASSERT_TEST(*list[0] == expected);
}

void AddItemToListThenGetItemUsingSubscriptOperator_GetsItem() {
    LinkedList<int> list;
    int expected = 1;
    list.Add(expected);

    int result = *list[0];

    ASSERT_TEST(result == expected);
}

void AddOneItemToList_ItemBecomesHead() {
    LinkedList<int> list;
    int expected = 2;
    list.Add(expected);

    int result = list.head->value;

    ASSERT_TEST(result == expected);
}

void AddOneItemToList_ItemBecomesTail() {
    LinkedList<int> list;
    int expected = 2;
    list.Add(expected);

    int result = list.tail->value;

    ASSERT_TEST(result == expected);
}

//void AddMultipleItemsToList