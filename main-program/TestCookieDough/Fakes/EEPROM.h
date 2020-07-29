#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

struct EEPROMClass
{
public:
    uint8_t read(int idx); // cookieOption: useVector
    void write(int idx, uint8_t val);
    void update(int idx, uint8_t val); // cookieOption: useVector

    uint16_t length();

    uint16_t &get(int idx, uint16_t &t); // cookieOption: useVector
    uint32_t &get(int idx, uint32_t &t); // cookieOption: useVector

    const uint8_t &put(int idx, const uint8_t &t); // cookieOption: useVector
    const uint16_t &put(int idx, const uint16_t &t); // cookieOption: useVector
    const uint32_t &put(int idx, const uint32_t &t); // cookieOption: useVector
};

static EEPROMClass EEPROM;

#endif