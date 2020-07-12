#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>

#define HIGH 0x1
#define LOW  0x0
#define INPUT_PULLUP 0x2
#define LED_BUILTIN 13

int digitalRead(uint8_t pin);
void pinMode(uint8_t pin, uint8_t mode);
unsigned long millis();
void digitalWrite(uint8_t pin, uint8_t val);
void delay(unsigned long ms);
double pow(double __x, double __y);

#include <stddef.h>
class Serial_ {
public:
    size_t print(const char *a);
    size_t print(char a);
    size_t print(unsigned char a, int b);
    size_t print(int a, int b);
    size_t print(unsigned int a, int b);
    size_t print(long a, int b);
    size_t print(unsigned long a, int b);
    size_t print(double a, int b);

    size_t println(const char *a);
    size_t println(char b);
    size_t println(unsigned char a, int b);
    size_t println(int a, int b);
    size_t println(unsigned int a, int b);
    size_t println(long a, int b);
    size_t println(unsigned long a, int b);
    size_t println(double a, int b);
    size_t println();
};

extern Serial_ Serial;
#endif
