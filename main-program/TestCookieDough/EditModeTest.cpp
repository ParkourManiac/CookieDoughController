#include "test.h"
#include "../lib/CookieDoughLibrary/EditMode.h"
//#include "Fakes/Arduino.h"

const int normalKeyCount = 4;

// void EditModeInitialized_NotEnabledByDefault() {
//     EditMode editmode = new EditMode(true);

//     ASSERT_TEST(editmode.enabled == false);
// }

extern size_t Serial__print_return;
extern unsigned int Serial__print_invocations;
extern const char * Serial__print_param_ifsh;

void SerialIsMocked_Works() {
    //Serial.print("hej");

    ASSERT_TEST(Serial__print_invocations == 1);
}