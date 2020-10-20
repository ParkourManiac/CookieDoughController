// TODO: Temporary solution. Should be replaced.
// Taken from https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory.


#ifndef TEST_ENVIRONMENT

#include "MeasureSRAM.h"

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


#endif