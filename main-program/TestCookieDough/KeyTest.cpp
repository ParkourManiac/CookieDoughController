#include "test.h"
#include "../lib/CookieDoughLibrary/Key.h"
#include <stdint.h>

extern uint8_t digitalReadInputPin;

void ConfigurePinAsKey_CallsDigitalReadWithRightPin() 
{
    int expectedPin = 2;
    Key key = Key(expectedPin, 1337);

    ASSERT_TEST(expectedPin == digitalReadInputPin);
}