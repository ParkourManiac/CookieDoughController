#include <stdint.h>

uint8_t digitalReadInputPin;
uint8_t digitalReadInputMode;
void digitalRead(uint8_t pin, uint8_t mode) {
    digitalReadInputPin = pin;
    digitalReadInputMode = mode;
}
void pinMode(uint8_t pin, uint8_t mode);
unsigned long millis();

#define INPUT_PULLUP 0x2
