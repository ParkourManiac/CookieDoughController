#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>

#define INPUT_PULLUP 0x2

int digitalRead(uint8_t pin);
void pinMode(uint8_t pin, uint8_t mode);
unsigned long millis();
void digitalWrite(uint8_t pin, uint8_t val);

#include <stddef.h>
class Serial_ {
public:
    size_t print(const char *ifsh);
};

//Serial_ Serial; < ------------------------- TODO CONTINUE HERE. This line causes problems...

#endif
