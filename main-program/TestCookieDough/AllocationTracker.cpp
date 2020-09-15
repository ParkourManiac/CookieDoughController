#ifdef USE_ALLOCATION_TRACKER_H

#include <iostream>
#include <memory>

#include "AllocationTracker.h"

AllocationTracker allocTracker;

uint32_t AllocationTracker::MemoryInUse()
{
    return allocatedMemory - freedMemory;
}

bool AllocationTracker::HasMemoryStateChanged() 
{
    return (oldMemoryInUse != MemoryInUse());
}

void AllocationTracker::SaveMemoryState()
{
    oldMemoryInUse = MemoryInUse();
}

uint32_t AllocationTracker::MemoryStateDifference() 
{
    return MemoryInUse() - oldMemoryInUse;
}

void *operator new(std::size_t size)
{
    // std::cout << "Allocating " << size << " bytes.\n";
    allocTracker.allocatedMemory += size;
    return malloc(size);
}

void operator delete(void *memory, std::size_t size) // NOTE: Only works in -std=c++14
{
    // std::cout << "Freeing " << size << " bytes.\n";
    allocTracker.freedMemory += size;
    free(memory);
}

#endif