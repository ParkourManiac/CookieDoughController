#ifdef USE_ALLOCATION_TRACKER_H
#ifndef ALLOCATION_TRACKER_H
#define ALLOCATION_TRACKER_H

#include <iostream>
#include <memory>

/**
 * @brief A simple tracker of allocated memory. 
 * Keeps track of the new and delete keyword allocations and deallocations.
 * Note: Overwrites the new and delete keyword.
 */
struct AllocationTracker
{
    uint32_t allocatedMemory = 0;
    uint32_t freedMemory = 0;
    uint32_t oldMemoryInUse = 0;

    uint32_t newCallCount = 0;
    uint32_t deleteCallCount = 0;
    uint32_t oldNewCallCount = 0;
    uint32_t oldDeleteCallCount = 0;
    uint32_t oldDiffNewDelete = 0;


    /**
     * @brief Retrieves the amount of memory currently allocated by the "new" keyword.
     * 
     * @return uint32_t The amount of memory currently in use.
     */
    uint32_t MemoryInUse();

    /**
     * @brief Returns the difference in call count between the new and delete operator. 
     * 
     * @return uint32_t The difference in call count between the new and delete operator.
     */
    uint32_t DifferenceNewDelete();

    /**
     * @brief Check if the amount of memory in use has changed since function "SaveMemoryState()" was called.
     * 
     * @return true The memory in use has changed in size.
     * @return false The memory is the same as when "SaveMemoryState()" was called.
     */
    bool HasMemoryStateChanged();

    /**
     * @brief Temporarily stores the number of current memory in use. Use this in conjunction with "HasMemoryStateChanged()".
     */
    void SaveMemoryState();

    /**
     * @brief Calculates the difference in memory that is used now compared to when "SaveMemoryState()" was called.
     * 
     * @return uint32_t The difference in amount of memory that was used compared to now.
     */
    uint32_t StateDifferenceMemory();

    /**
     * @brief Returns the amount of times the new operator has been called since the function "SaveMemoryState()" was called.
     * 
     * @return uint32_t The amount of times new was called.
     */
    uint32_t StateDifferenceNewCount();

    /**
     * @brief Returns the amount of times the delete operator has been called since the function "SaveMemoryState()" was called.
     * 
     * @return uint32_t The amount of times delete was called.
     */
    uint32_t StateDifferenceDeleteCount();

};

#endif
#endif
