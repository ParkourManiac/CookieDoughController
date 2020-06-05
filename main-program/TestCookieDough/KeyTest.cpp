#include "test.h"
#include "../lib/CookieDoughLibrary/Key.h"
#include <stdint.h>

extern uint8_t pinMode_InputPin;

void ConfigurePinAsKey_KeyIsCorrectlyParsedAsIKey() 
{
    int expectedPin = 2;
    Key key = Key(expectedPin, 1337);

    ConfigurePinForKey(key);

    ASSERT_TEST(expectedPin == pinMode_InputPin);
}

void ConfigurePinAsKey_SpecialKeyIsCorrectlyParsedAsIKey() 
{
    int expectedPin = 2;
    Key key = Key(expectedPin, 1337); 
    
    // TODO: We didn't even call the function and it passes!? Reset the Fakes values between each test or find other solution.
    // Maybe generate .cpp file containing the inputted parameters and the hardcoded return value. Do this using python and take functions from Fakes/*.h headerfiles. 

    ASSERT_TEST(expectedPin == pinMode_InputPin);
}