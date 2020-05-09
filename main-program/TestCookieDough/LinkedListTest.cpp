#include "test.h"
#include "../InDevelopment/LinkedList.cpp"

void GetFirstNode_GetsFirstNode()
{
    LinkedList<int> list;
    int expected = 1337;

    list.Add(expected);

    ASSERT_TEST(list.GetNode(0)->value == expected);
}

void GetSecondNode_GetsSecondNode()
{
    LinkedList<int> list;
    int expected = 1337;
    list.Add(1);
    list.Add(expected);

    ASSERT_TEST(list.GetNode(1)->value == expected);
}

void GetNodeInTheMiddleOfList_GetsNode() {
    LinkedList<int> list;
    int expected = 1337;
    list.Add(1);
    list.Add(2);
    list.Add(expected);
    list.Add(4);
    list.Add(5);

    ASSERT_TEST(list.GetNode(2)->value == expected);
}

void GetLastNodeByPassingInLengthMinusOne_GetsLastNode() {
    LinkedList<int> list;
    int expected = 1337;

    for(int i = 0; i < 7; i++) {
        list.Add(i);
    }
    list.Add(expected);

    ASSERT_TEST(list.GetNode(list.length - 1)->value == expected);
}

void TryToGetNodeOutOfRange_GetsNullptr() {
    LinkedList<int> list;

    list.Add(1);

    ASSERT_TEST(list.GetNode(list.length + 10) == nullptr);
}

void TryToGetNodeWithEmptyList_GetsNullptr() {
    LinkedList<int> list;

    ASSERT_TEST(list.GetNode(0) == nullptr);
}

void TryToGetNodeWithNegativeNumber_GetsNullptr() {
    LinkedList<int> list;

    list.Add(1);

    ASSERT_TEST(list.GetNode(-1) == nullptr);
}

void GetTwoDifferentNodes_AllNodesAreNotTheSame() {
    LinkedList<int> list;

    list.Add(1);
    list.Add(2);

    ASSERT_TEST(list.GetNode(0)->value != list.GetNode(1)->value);
}

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

    ASSERT_TEST(*list[1] == expected);
}

void GetItemInTheMiddleOfListUsingSubscriptOperator_GetsItem() {
    LinkedList<int> list;
    int expected = 1337;
    list.Add(1);
    list.Add(2);
    list.Add(expected);
    list.Add(4);
    list.Add(5);

    ASSERT_TEST(*list[2] == expected);
}

void GetLastItemByPassingLengthMinusOneToSubscriptOperator_GetsLastItem() {
    LinkedList<int> list;
    int expected = 1337;

    for(int i = 0; i < 7; i++) {
        list.Add(i);
    }
    list.Add(expected);

    ASSERT_TEST(*list[list.length - 1] == expected);
}

void OutOfRangeInSubscriptOperator_GetsNullptr() {
    LinkedList<int> list;

    list.Add(1);

    ASSERT_TEST(list[list.length + 10] == nullptr);
}

void UsingSubscriptOperatorWithEmptyList_GetsNullptr() {
    LinkedList<int> list;

    ASSERT_TEST(list[0] == nullptr);
}

void UsingSubscriptOperatorWithNegativeNumber_GetsNullptr() {
    LinkedList<int> list;

    list.Add(1);

    ASSERT_TEST(list[-1] == nullptr);
}

void UsingSubscriptOperator_AllItemsAreNotTheSame() {
    LinkedList<int> list;

    list.Add(1);
    list.Add(2);

    ASSERT_TEST(*list[0] != *list[1]);
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

void AddItem_LengthIncreasesByOne() {
    LinkedList<int> list;
    int expected = 1;

    list.Add(1337);

    ASSERT_TEST(list.length == expected);
}

void InsertItemIntoEmptyList_ItemIsHead() {
    LinkedList<int> list;
    int expected = 1337;

    list.Insert(0, expected);

    ASSERT_TEST(list.head->value == expected);
}

void InsertItemIntoEmptyList_ItemIsTail() {
    LinkedList<int> list;
    int expected = 1337;

    list.Insert(0, expected);

    ASSERT_TEST(list.tail->value == expected);
}

void InsertItemAtIndexZero_ItemIsHead() {
    LinkedList<int> list;
    int expected = 1337;

    list.Insert(0, 1);
    list.Insert(0, 2);
    list.Insert(0, expected);

    ASSERT_TEST(list.head->value == expected);
}

void InsertItemAtLastIndex_ItemIsTail() {
    LinkedList<int> list;
    int expected = 1337;

    list.Insert(0, 1);
    list.Insert(0, 2);
    list.Insert(0, 3);
    list.Insert(0, 4);
    list.Insert(4, expected);

    ASSERT_TEST(list.tail->value == expected);
}

void InsertItemAtIndexThenGetItemFromIndex_ItemAtIndexIsTheInsertedItem() {
    LinkedList<int> list;
    int expectedItem = 1337;
    int expectedIndex = 2;
    list.Insert(0, 1);
    list.Insert(0, 1);
    list.Insert(0, 1);
    list.Insert(0, 1);

    list.Insert(expectedIndex, expectedItem);

    ASSERT_TEST(*list[expectedIndex] == expectedItem);
}

void InsertItemIntoMiddleOfPopulatedList_AllItemsArePresent() {
    LinkedList<int> list;
    int expectedItem1 = 1337;
    int expectedItem2 = 42;
    int expectedItem3 = 84;
    list.Insert(0, expectedItem1);
    list.Insert(1, expectedItem3);

    list.Insert(1, expectedItem2);

    ASSERT_TEST(*list[0] == expectedItem1 &&
                *list[1] == expectedItem2 &&
                *list[2] == expectedItem3);
}

void InsertItemIntoEmptyListWithIndexOutOfRange_NothingIsAdded() {
    LinkedList<int> list;
    int expectedIndex = 24;

    list.Insert(expectedIndex, 1337);

    ASSERT_TEST(list[expectedIndex] == nullptr && list[0] == nullptr);
}

void InsertItemIntoPopulatedListWithIndexOutOfRange_NothingIsAdded() {
    LinkedList<int> list;
    int expectedIndex = 24;

    list.Insert(0, 1);
    list.Insert(0, 2);
    list.Insert(0, 3);

    list.Insert(expectedIndex, 1337);

    ASSERT_TEST(list[expectedIndex] == nullptr && list[3] == nullptr);
}

void InsertItem_LengthIncreasesByOne() {
    LinkedList<int> list;

    list.Insert(0, 1);

    ASSERT_TEST(list.length == 1);
}

void InsertMultipleItems_LengthIsSameAsAmountOfItems() {
    LinkedList<int> list;
    int expectedSize;

    for(int i = 0; i < expectedSize; i++) {
        list.Insert(0, i);
    }

    ASSERT_TEST(list.length == expectedSize);
}

void InsertItemAtLastIndexUsingLength_ItemIsTail() {
    LinkedList<int> list;
    int expected = 1337;
    list.Insert(0, 1);
    list.Insert(0, 2);
    list.Insert(0, 3);

    list.Insert(list.length, expected);

    ASSERT_TEST(list.tail->value == expected);

}