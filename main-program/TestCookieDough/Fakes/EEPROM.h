#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

struct EEPROMClass
{
public:
    uint8_t read(int idx); // cookieOption: useVector
    void write(int idx, uint8_t val);
    void update(int idx, uint8_t val); // cookieOption: useVector

    uint16_t length(); // cookieOption: useVector

    uint16_t &get(int idx, uint16_t &t); // cookieOption: useVector, overwriteReference
    uint32_t &get(int idx, uint32_t &t); // cookieOption: useVector, overwriteReference

    const uint8_t &put(int idx, const uint8_t &t); // cookieOption: useVector
    const uint16_t &put(int idx, const uint16_t &t); // cookieOption: useVector
    const uint32_t &put(int idx, const uint32_t &t); // cookieOption: useVector
};

#pragma GCC diagnostic ignored "-Wunused-variable"
static EEPROMClass EEPROM;
#pragma GCC diagnostic pop

#endif