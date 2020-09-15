// #ifndef ALLOCATION_TRACKER_H
// #define ALLOCATION_TRACKER_H

// #include <iostream>
// #include <memory>

// void* operator new(std::size_t size) 
// {
//     std::cout << "Allocating " << size << " bytes.\n";
//     return malloc(size);
// }

// void operator delete(void* memory, std::size_t size) // NOTE: Only works in std-14
// {
//     std::cout << "Freeing " << size << " bytes.\n";
//     free(memory);
// }


// #endif