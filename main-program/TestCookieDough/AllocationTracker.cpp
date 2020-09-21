#ifdef USE_ALLOCATION_TRACKER_H

#include <iostream>
#include <memory>

#include "AllocationTracker.h"

AllocationTracker allocTracker;

uint32_t AllocationTracker::MemoryInUse()
{
    return allocatedMemory - freedMemory;
}

uint32_t AllocationTracker::DifferenceNewDelete() 
{
    return newCallCount - deleteCallCount;
}

bool AllocationTracker::HasMemoryStateChanged()
{
    return (oldMemoryInUse != MemoryInUse()) || (oldDiffNewDelete != DifferenceNewDelete());
}

void AllocationTracker::SaveMemoryState()
{
    oldMemoryInUse = MemoryInUse();
    oldNewCallCount = newCallCount;
    oldDeleteCallCount = deleteCallCount;
    oldDiffNewDelete = DifferenceNewDelete();
}

uint32_t AllocationTracker::StateDifferenceMemory() 
{
    return MemoryInUse() - oldMemoryInUse;
}

uint32_t AllocationTracker::StateDifferenceNewCount() 
{
    return newCallCount - oldNewCallCount;
}

uint32_t AllocationTracker::StateDifferenceDeleteCount() 
{
    return deleteCallCount - oldDeleteCallCount;
}

void *operator new(std::size_t size)
{
    // std::cout << "Allocating " << size << " bytes.\n";
    allocTracker.allocatedMemory += size;
    allocTracker.newCallCount += 1;
    return malloc(size);
}

void operator delete(void *memory, std::size_t size) // NOTE: Only works in -std=c++14
{
    // std::cout << "Freeing " << size << " bytes.\n";
    allocTracker.freedMemory += size;
    allocTracker.deleteCallCount += 1;
    free(memory);
}

#endif