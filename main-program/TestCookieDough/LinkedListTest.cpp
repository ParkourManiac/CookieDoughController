#include "test.h"
#include "../lib/CookieDoughLibrary/LinkedList.h"
#include "../lib/CookieDoughLibrary/LinkedList.cpp"

void GetFirstNode_GetsFirstNode()
{
    LinkedList<int> list;
    int expected = 1337;

    list.Add(expected);

    ASSERT_TEST(list.GetNodeAtIndex(0)->value == expected);
}

void GetSecondNode_GetsSecondNode()
{
    LinkedList<int> list;
    int expected = 1337;
    list.Add(1);
    list.Add(expected);

    ASSERT_TEST(list.GetNodeAtIndex(1)->value == expected);
}

void GetNodeInTheMiddleOfList_GetsNode()
{
    LinkedList<int> list;
    int expected = 1337;
    list.Add(1);
    list.Add(2);
    list.Add(expected);
    list.Add(4);
    list.Add(5);

    ASSERT_TEST(list.GetNodeAtIndex(2)->value == expected);
}

void GetLastNodeByPassingInLengthMinusOne_GetsLastNode()
{
    LinkedList<int> list;
    int expected = 1337;

    for (int i = 0; i < 7; i++)
    {
        list.Add(i);
    }
    list.Add(expected);

    ASSERT_TEST(list.GetNodeAtIndex(list.length - 1)->value == expected);
}

void TryToGetNodeOutOfRange_GetsNullptr()
{
    LinkedList<int> list;

    list.Add(1);

    ASSERT_TEST(list.GetNodeAtIndex(list.length + 10) == nullptr);
}

void TryToGetNodeWithEmptyList_GetsNullptr()
{
    LinkedList<int> list;

    ASSERT_TEST(list.GetNodeAtIndex(0) == nullptr);
}

void TryToGetNodeWithNegativeNumber_GetsNullptr()
{
    LinkedList<int> list;

    list.Add(1);

    ASSERT_TEST(list.GetNodeAtIndex(-1) == nullptr);
}

void GetTwoDifferentNodes_AllNodesAreNotTheSame()
{
    LinkedList<int> list;

    list.Add(1);
    list.Add(2);

    ASSERT_TEST(list.GetNodeAtIndex(0)->value != list.GetNodeAtIndex(1)->value);
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

void GetItemInTheMiddleOfListUsingSubscriptOperator_GetsItem()
{
    LinkedList<int> list;
    int expected = 1337;
    list.Add(1);
    list.Add(2);
    list.Add(expected);
    list.Add(4);
    list.Add(5);

    ASSERT_TEST(*list[2] == expected);
}

void GetLastItemByPassingLengthMinusOneToSubscriptOperator_GetsLastItem()
{
    LinkedList<int> list;
    int expected = 1337;

    for (int i = 0; i < 7; i++)
    {
        list.Add(i);
    }
    list.Add(expected);

    ASSERT_TEST(*list[list.length - 1] == expected);
}

void OutOfRangeInSubscriptOperator_GetsNullptr()
{
    LinkedList<int> list;

    list.Add(1);

    ASSERT_TEST(list[list.length + 10] == nullptr);
}

void UsingSubscriptOperatorWithEmptyList_GetsNullptr()
{
    LinkedList<int> list;

    ASSERT_TEST(list[0] == nullptr);
}

void UsingSubscriptOperatorWithNegativeNumber_GetsNullptr()
{
    LinkedList<int> list;

    list.Add(1);

    ASSERT_TEST(list[-1] == nullptr);
}

void UsingSubscriptOperator_AllItemsAreNotTheSame()
{
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

void AddItem_LengthIncreasesByOne()
{
    LinkedList<int> list;
    int expected = 1;

    list.Add(1337);

    ASSERT_TEST(list.length == expected);
}

void InsertItemIntoEmptyList_ItemIsHead()
{
    LinkedList<int> list;
    int expected = 1337;

    list.Insert(0, expected);

    ASSERT_TEST(list.head->value == expected);
}

void InsertItemIntoEmptyList_ItemIsTail()
{
    LinkedList<int> list;
    int expected = 1337;

    list.Insert(0, expected);

    ASSERT_TEST(list.tail->value == expected);
}

void InsertItemAtIndexZero_ItemIsHead()
{
    LinkedList<int> list;
    int expected = 1337;

    list.Insert(0, 1);
    list.Insert(0, 2);
    list.Insert(0, expected);

    ASSERT_TEST(list.head->value == expected);
}

void InsertItemAtLastIndex_ItemIsTail()
{
    LinkedList<int> list;
    int expected = 1337;

    list.Insert(0, 1);
    list.Insert(0, 2);
    list.Insert(0, 3);
    list.Insert(0, 4);
    list.Insert(4, expected);

    ASSERT_TEST(list.tail->value == expected);
}

void InsertItemAtIndexThenGetItemFromIndex_ItemAtIndexIsTheInsertedItem()
{
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

void InsertItemIntoMiddleOfPopulatedList_AllItemsArePresent()
{
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

void InsertItemIntoEmptyListWithIndexOutOfRange_NothingIsAdded()
{
    LinkedList<int> list;
    int expectedIndex = 24;

    list.Insert(expectedIndex, 1337);

    ASSERT_TEST(list[expectedIndex] == nullptr && list[0] == nullptr);
}

void InsertItemIntoPopulatedListWithIndexOutOfRange_NothingIsAdded()
{
    LinkedList<int> list;
    int expectedIndex = 24;

    list.Insert(0, 1);
    list.Insert(0, 2);
    list.Insert(0, 3);

    list.Insert(expectedIndex, 1337);

    ASSERT_TEST(list[expectedIndex] == nullptr && list[3] == nullptr);
}

void InsertItem_LengthIncreasesByOne()
{
    LinkedList<int> list;

    list.Insert(0, 1);

    ASSERT_TEST(list.length == 1);
}

void InsertMultipleItems_LengthIsSameAsAmountOfItems()
{
    LinkedList<int> list;
    int expectedSize = 10;

    for (int i = 0; i < expectedSize; i++)
    {
        list.Insert(0, i);
    }

    ASSERT_TEST(list.length == expectedSize);
}

void InsertItemAtLastIndexUsingLength_ItemIsTail()
{
    LinkedList<int> list;
    int expected = 1337;
    list.Insert(0, 1);
    list.Insert(0, 2);
    list.Insert(0, 3);

    list.Insert(list.length, expected);

    ASSERT_TEST(list.tail->value == expected);
}

void TryToRemoveItemOfEmptyList_ReturnsFalse()
{
    LinkedList<int> list;

    ASSERT_TEST(list.RemoveAtIndex(0) == false);
}

void TryToRemoveItemOutOfRange_ReturnsFalse()
{
    LinkedList<int> list;
    list.Add(1);
    list.Add(2);
    list.Add(3);

    ASSERT_TEST(list.RemoveAtIndex(3) == false);
}

void RemovesSpecificItemFromList_ReturnsCorrectItem() {
    LinkedList<int> list;
    int expectedValue = 1337;
    int expectedIndex = 3;
    list.Add(1);
    list.Add(2);
    list.Add(3);
    list.Add(4);
    list.Add(5);

    list.Insert(expectedIndex, expectedValue);
    int *result = new int;
    list.RemoveAtIndex(expectedIndex, result);

    ASSERT_TEST(*result == expectedValue);
    delete(result);
}

void RemoveOnlyItemInList_ReturnsCorrectItem() {
    LinkedList<int> list;
    int expected = 1337;
    list.Add(expected);
    
    int *result = new int;
    list.RemoveAtIndex(0, result);

    ASSERT_TEST(*result == expected);
    delete(result);
}

void RemoveOnlyItemInList_HeadIsNullptr()
{
    LinkedList<int> list;
    list.Add(1);
    
    list.RemoveAtIndex(0);

    ASSERT_TEST(list.head == nullptr);
}

void RemoveOnlyItemInList_TailIsNullptr()
{
    LinkedList<int> list;
    list.Add(1);
    
    list.RemoveAtIndex(0);

    ASSERT_TEST(list.tail == nullptr);
}

void RemoveFirstItemOfListWithTwoItems_SecondItemIsNowHead() {
    LinkedList<int> list;
    int firstItem = 1;
    int secondItem = 1337;
    list.Add(firstItem);
    list.Add(secondItem);

    list.RemoveAtIndex(0);

    ASSERT_TEST(list.head->value == secondItem);
}

void RemoveFirstItemOfListWithTwoItems_SecondItemIsStillTail() {
    LinkedList<int> list;
    int firstItem = 1;
    int secondItem = 1337;
    list.Add(firstItem);
    list.Add(secondItem);

    list.RemoveAtIndex(0);

    ASSERT_TEST(list.tail->value == secondItem);
}

void RemoveItemInTheMiddleOfList_ItemFollowingTheRemovedItemNowOccupiesTheRemovedItemsIndex() {
    LinkedList<int> list;
    int indexToBeRemoved = 1;
    int expectedFollowingItem = 1337;
    list.Add(1);
    list.Add(1);
    list.Add(1);
    list.Add(1);
    list.Add(1);
    list.Insert(indexToBeRemoved + 1, expectedFollowingItem);

    list.RemoveAtIndex(indexToBeRemoved);

    ASSERT_TEST(*list[indexToBeRemoved] == expectedFollowingItem);
}

void RemoveItemInTheMiddleOfList_TheRemovedItemIsNoLongerInTheListAtTheRemovedIndex() {
    LinkedList<int> list;
    int indexToBeRemoved = 1;
    int expectedRemovedItem = 1337;
    list.Add(1);
    list.Add(1);
    list.Add(1);
    list.Add(1);
    list.Insert(indexToBeRemoved, expectedRemovedItem);

    list.RemoveAtIndex(indexToBeRemoved);

    ASSERT_TEST(*list[indexToBeRemoved] != expectedRemovedItem);
}

void RemoveItemInMiddleOfList_FollowingItemTakesRemovedItemsPosition() {
    LinkedList<int> list;
    int indexToBeRemoved = 1;
    int followingItem = 1337;
    list.Add(0);
    list.Add(1);
    list.Add(3);
    list.Insert(indexToBeRemoved + 1, followingItem);

    list.RemoveAtIndex(indexToBeRemoved);

    ASSERT_TEST(*list[indexToBeRemoved] == followingItem);
}

void RemoveLastItemInList_SecondLastItemIsNowTail() {
    LinkedList<int> list;
    int secondLastItem = 1337;
    int lastItemInList = 42;
    list.Add(1);
    list.Add(2);
    list.Add(secondLastItem);
    list.Add(lastItemInList);

    list.RemoveAtIndex(3);

    ASSERT_TEST(list.tail->value == secondLastItem);
}

void RemoveLastItemInList_NewTailsNextPointerIsNowNullptr() {
    LinkedList<int> list;
    list.Add(1);
    list.Add(2);
    list.Add(3);
    list.Add(4);

    list.RemoveAtIndex(3);

    ASSERT_TEST(list.tail->next == nullptr);
}

void RemoveOnlyItemInList_LengthIsZero() {
    LinkedList<int> list;
    list.Add(0);

    list.RemoveAtIndex(0);

    ASSERT_TEST(list.length == 0);
}

void RemoveItemInPopulatedList_LengthDecreasesByOne() {
    LinkedList<int> list;
    int amountOfAddedItems = 7;
    int indexToRemove = 3;

    for(int i = 0; i < amountOfAddedItems; i++) {
        list.Add(1);
    }

    list.RemoveAtIndex(indexToRemove);

    ASSERT_TEST(list.length == amountOfAddedItems - 1);
}

void RemoveAllItemsInList_LengthIsZero() {
    LinkedList<int> list;
    list.Add(1);
    list.Add(2);
    list.Add(3);

    list.RemoveAtIndex(0);
    list.RemoveAtIndex(0);
    list.RemoveAtIndex(0);

    ASSERT_TEST(list.length == 0);
}

void ClearList_HeadIsNullptr() {
    LinkedList<int> list;
    list.Add(1);
    list.Add(2);
    list.Add(3);

    list.Clear();

    ASSERT_TEST(list.head == nullptr);
}

void ClearList_TailIsNullptr() {
    LinkedList<int> list;
    list.Add(1);
    list.Add(2);
    list.Add(3);

    list.Clear();

    ASSERT_TEST(list.tail == nullptr);
}

void ClearList_LengthIsZero() {
    LinkedList<int> list;
    list.Add(1);
    list.Add(2);
    list.Add(3);

    list.Clear();
    
    ASSERT_TEST(list.length == 0);
}

void CheckIsEmptyWhenListHasNoItems_ReturnsTrue() {
    LinkedList<int> list;
    ASSERT_TEST(list.IsEmpty() == true);
}

void CheckIsEmptyWhenNotEmpty_ReturnsFalse() {
    LinkedList<int> list;

    list.Add(0);

    ASSERT_TEST(list.IsEmpty() == false);
}

void CheckIsEmptyAfterInsertingItem_ReturnsFalse() {
    LinkedList<int> list;

    list.Insert(0, 0);

    ASSERT_TEST(list.IsEmpty() == false);
}

void CheckIsEmptyAfterBecomingEmpty_ReturnsTrue() {
    LinkedList<int> list;

    list.Add(0);
    list.RemoveAtIndex(0);

    ASSERT_TEST(list.IsEmpty() == true);
}

void CheckIsEmptyAfterAddingMultipleItemsThenRemovingOne_ReturnsFalse() {
    LinkedList<int> list;

    list.Add(0);
    list.Add(1);
    list.Add(2);
    list.RemoveAtIndex(0);

    ASSERT_TEST(list.IsEmpty() == false);
}

// TODO: void ClearList_AllMemoryIsFreed()