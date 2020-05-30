// THIS FILE WAS AUTOGENERATED BY FILE generateTestSuite.py
#include "testSuite.h" 
#include "test.h"

void GetFirstNode_GetsFirstNode();
void GetSecondNode_GetsSecondNode();
void GetNodeInTheMiddleOfList_GetsNode();
void GetLastNodeByPassingInLengthMinusOne_GetsLastNode();
void TryToGetNodeOutOfRange_GetsNullptr();
void TryToGetNodeWithEmptyList_GetsNullptr();
void TryToGetNodeWithNegativeNumber_GetsNullptr();
void GetTwoDifferentNodes_AllNodesAreNotTheSame();
void GetFirstItemUsingSubscriptOperator_GetsItem();
void GetSecondItemUsingSubscriptOperator_GetsSecondItem();
void GetItemInTheMiddleOfListUsingSubscriptOperator_GetsItem();
void GetLastItemByPassingLengthMinusOneToSubscriptOperator_GetsLastItem();
void OutOfRangeInSubscriptOperator_GetsNullptr();
void UsingSubscriptOperatorWithEmptyList_GetsNullptr();
void UsingSubscriptOperatorWithNegativeNumber_GetsNullptr();
void UsingSubscriptOperator_AllItemsAreNotTheSame();
void AddItemToList_ListContainsItem();
void AddOneItemToList_ItemBecomesHead();
void AddOneItemToList_ItemBecomesTail();
void AddTwoItemsToList_FirstIsHeadSecondIsTail();
void AddMultipleItemsToList_FirstlyAddedIsHeadLastlyAddedIsTail();
void AddItem_LengthIncreasesByOne();
void InsertItemIntoEmptyList_ItemIsHead();
void InsertItemIntoEmptyList_ItemIsTail();
void InsertItemAtIndexZero_ItemIsHead();
void InsertItemAtLastIndex_ItemIsTail();
void InsertItemAtIndexThenGetItemFromIndex_ItemAtIndexIsTheInsertedItem();
void InsertItemIntoMiddleOfPopulatedList_AllItemsArePresent();
void InsertItemIntoEmptyListWithIndexOutOfRange_NothingIsAdded();
void InsertItemIntoPopulatedListWithIndexOutOfRange_NothingIsAdded();
void InsertItem_LengthIncreasesByOne();
void InsertMultipleItems_LengthIsSameAsAmountOfItems();
void InsertItemAtLastIndexUsingLength_ItemIsTail();
void TryToRemoveItemOfEmptyList_ReturnsFalse();
void TryToRemoveItemOutOfRange_ReturnsFalse();
void RemovesSpecificItemFromList_ReturnsCorrectItem();
void RemoveOnlyItemInList_ReturnsCorrectItem();
void RemoveOnlyItemInList_HeadIsNullptr();
void RemoveOnlyItemInList_TailIsNullptr();
void RemoveFirstItemOfListWithTwoItems_SecondItemIsNowHead();
void RemoveFirstItemOfListWithTwoItems_SecondItemIsStillTail();
void RemoveItemInTheMiddleOfList_ItemFollowingTheRemovedItemNowOccupiesTheRemovedItemsIndex();
void RemoveItemInTheMiddleOfList_TheRemovedItemIsNoLongerInTheListAtTheRemovedIndex();
void RemoveItemInMiddleOfList_FollowingItemTakesRemovedItemsPosition();
void RemoveLastItemInList_SecondLastItemIsNowTail();
void RemoveLastItemInList_NewTailsNextPointerIsNowNullptr();
void RemoveOnlyItemInList_LengthIsZero();
void RemoveItemInPopulatedList_LengthDecreasesByOne();
void RemoveAllItemsInList_LengthIsZero();
void ClearList_HeadIsNullptr();
void ClearList_TailIsNullptr();
void ClearList_LengthIsZero();
void CheckIsEmptyWhenListHasNoItems_ReturnsTrue();
void CheckIsEmptyWhenNotEmpty_ReturnsFalse();
void CheckIsEmptyAfterInsertingItem_ReturnsFalse();
void CheckIsEmptyAfterBecomingEmpty_ReturnsTrue();
void CheckIsEmptyAfterAddingMultipleItemsThenRemovingOne_ReturnsFalse();
void ConfigurePinAsKey_KeyIsCorrectlyParsedAsIKey();
void ConfigurePinAsKey_SpecialKeyIsCorrectlyParsedAsIKey();

void RunTests() 
{
	RUN_TEST(GetFirstNode_GetsFirstNode);
	RUN_TEST(GetSecondNode_GetsSecondNode);
	RUN_TEST(GetNodeInTheMiddleOfList_GetsNode);
	RUN_TEST(GetLastNodeByPassingInLengthMinusOne_GetsLastNode);
	RUN_TEST(TryToGetNodeOutOfRange_GetsNullptr);
	RUN_TEST(TryToGetNodeWithEmptyList_GetsNullptr);
	RUN_TEST(TryToGetNodeWithNegativeNumber_GetsNullptr);
	RUN_TEST(GetTwoDifferentNodes_AllNodesAreNotTheSame);
	RUN_TEST(GetFirstItemUsingSubscriptOperator_GetsItem);
	RUN_TEST(GetSecondItemUsingSubscriptOperator_GetsSecondItem);
	RUN_TEST(GetItemInTheMiddleOfListUsingSubscriptOperator_GetsItem);
	RUN_TEST(GetLastItemByPassingLengthMinusOneToSubscriptOperator_GetsLastItem);
	RUN_TEST(OutOfRangeInSubscriptOperator_GetsNullptr);
	RUN_TEST(UsingSubscriptOperatorWithEmptyList_GetsNullptr);
	RUN_TEST(UsingSubscriptOperatorWithNegativeNumber_GetsNullptr);
	RUN_TEST(UsingSubscriptOperator_AllItemsAreNotTheSame);
	RUN_TEST(AddItemToList_ListContainsItem);
	RUN_TEST(AddOneItemToList_ItemBecomesHead);
	RUN_TEST(AddOneItemToList_ItemBecomesTail);
	RUN_TEST(AddTwoItemsToList_FirstIsHeadSecondIsTail);
	RUN_TEST(AddMultipleItemsToList_FirstlyAddedIsHeadLastlyAddedIsTail);
	RUN_TEST(AddItem_LengthIncreasesByOne);
	RUN_TEST(InsertItemIntoEmptyList_ItemIsHead);
	RUN_TEST(InsertItemIntoEmptyList_ItemIsTail);
	RUN_TEST(InsertItemAtIndexZero_ItemIsHead);
	RUN_TEST(InsertItemAtLastIndex_ItemIsTail);
	RUN_TEST(InsertItemAtIndexThenGetItemFromIndex_ItemAtIndexIsTheInsertedItem);
	RUN_TEST(InsertItemIntoMiddleOfPopulatedList_AllItemsArePresent);
	RUN_TEST(InsertItemIntoEmptyListWithIndexOutOfRange_NothingIsAdded);
	RUN_TEST(InsertItemIntoPopulatedListWithIndexOutOfRange_NothingIsAdded);
	RUN_TEST(InsertItem_LengthIncreasesByOne);
	RUN_TEST(InsertMultipleItems_LengthIsSameAsAmountOfItems);
	RUN_TEST(InsertItemAtLastIndexUsingLength_ItemIsTail);
	RUN_TEST(TryToRemoveItemOfEmptyList_ReturnsFalse);
	RUN_TEST(TryToRemoveItemOutOfRange_ReturnsFalse);
	RUN_TEST(RemovesSpecificItemFromList_ReturnsCorrectItem);
	RUN_TEST(RemoveOnlyItemInList_ReturnsCorrectItem);
	RUN_TEST(RemoveOnlyItemInList_HeadIsNullptr);
	RUN_TEST(RemoveOnlyItemInList_TailIsNullptr);
	RUN_TEST(RemoveFirstItemOfListWithTwoItems_SecondItemIsNowHead);
	RUN_TEST(RemoveFirstItemOfListWithTwoItems_SecondItemIsStillTail);
	RUN_TEST(RemoveItemInTheMiddleOfList_ItemFollowingTheRemovedItemNowOccupiesTheRemovedItemsIndex);
	RUN_TEST(RemoveItemInTheMiddleOfList_TheRemovedItemIsNoLongerInTheListAtTheRemovedIndex);
	RUN_TEST(RemoveItemInMiddleOfList_FollowingItemTakesRemovedItemsPosition);
	RUN_TEST(RemoveLastItemInList_SecondLastItemIsNowTail);
	RUN_TEST(RemoveLastItemInList_NewTailsNextPointerIsNowNullptr);
	RUN_TEST(RemoveOnlyItemInList_LengthIsZero);
	RUN_TEST(RemoveItemInPopulatedList_LengthDecreasesByOne);
	RUN_TEST(RemoveAllItemsInList_LengthIsZero);
	RUN_TEST(ClearList_HeadIsNullptr);
	RUN_TEST(ClearList_TailIsNullptr);
	RUN_TEST(ClearList_LengthIsZero);
	RUN_TEST(CheckIsEmptyWhenListHasNoItems_ReturnsTrue);
	RUN_TEST(CheckIsEmptyWhenNotEmpty_ReturnsFalse);
	RUN_TEST(CheckIsEmptyAfterInsertingItem_ReturnsFalse);
	RUN_TEST(CheckIsEmptyAfterBecomingEmpty_ReturnsTrue);
	RUN_TEST(CheckIsEmptyAfterAddingMultipleItemsThenRemovingOne_ReturnsFalse);
	RUN_TEST(ConfigurePinAsKey_KeyIsCorrectlyParsedAsIKey);
	RUN_TEST(ConfigurePinAsKey_SpecialKeyIsCorrectlyParsedAsIKey);
}


void ResetMocks() 
{
}

