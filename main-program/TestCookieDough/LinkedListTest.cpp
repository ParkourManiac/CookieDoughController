#include "test.h"
#include "../InDevelopment/LinkedList.cpp"

void AddItemToList_ListContainsItem() {
    LinkedList<int> list;
    int expected = 1;

    list.Add(expected);

    ASSERT_TEST(list[0] = expected);
}