#include "Arduino.h"

uint8_t pinMode_InputPin;
uint8_t pinMode_InputMode;
void pinMode(uint8_t pin, uint8_t mode)
{
    pinMode_InputPin = pin;
    pinMode_InputMode = mode;
}

int digitalRead_ReturnInt;
uint8_t digitalRead_InputPin;
int digitalRead(uint8_t pin) { return digitalRead_ReturnInt; }

unsigned long millis_ReturnUnsignedLong;
unsigned long millis() { return millis_ReturnUnsignedLong; }