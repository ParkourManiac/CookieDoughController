#include "test.h"
#include "../lib/CookieDoughLibrary/EditMode.h"
#include "Fakes/Arduino.h"

const int normalKeyCount = 4;

void EditModeInitialized_NotEnabledByDefault() { // TODO: Write a few test for EditMode using class mock to test that it's working. Then create a new testSuite_WORKING.txt Test.
    EditMode editmode = new EditMode(true);

    ASSERT_TEST(editmode.enabled == false);
}

extern size_t Serial__print_return_o1;
extern unsigned int Serial__print_invocations_o1;
extern const char * Serial__print_param_ifsh_o1;

void SerialIsMocked_Works() {
    Serial.print("hej");

    ASSERT_TEST(Serial__print_invocations_o1 == 1);
}