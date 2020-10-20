// TODO: Temporary solution. Should be replaced.
// Taken from https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory.


#ifndef TEST_ENVIRONMENT

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

#endif


int freeMemory();

