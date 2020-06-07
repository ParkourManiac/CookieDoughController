#include "test.h"
#include "../lib/CookieDoughLibrary/Key.h"
#include <stdint.h>

extern uint8_t pinMode_param_pin;
extern unsigned int pinMode_invocations;

void ConfigurePinAsKey_PinOfTypeKeyIsCorrectlyParsedToIKey() 
{
    int expectedPin = 2;
    Key key = Key(expectedPin, 1337);

    ConfigurePinForKey(key);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinAsKey_PinOfSpecialKeyIsCorrectlyParsedToIKey() 
{
    int expectedPin = 2;
    SpecialKey key = SpecialKey(expectedPin, toggleDefaultKeyMap); 

    ConfigurePinForKey(key);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinAsKey_IKeysPinIsPassedToPinMode() 
{
    int expectedPin = 2;
    IKey key = IKey { .pin = expectedPin}; 

    ConfigurePinForKey(key);

    ASSERT_TEST(expectedPin == pinMode_param_pin);
}

void ConfigurePinAsKey_CallsPinModeOnce() 
{
    SpecialKey key = SpecialKey(2, toggleDefaultKeyMap); 

    ConfigurePinForKey(key);

    ASSERT_TEST(1 == pinMode_invocations);
}