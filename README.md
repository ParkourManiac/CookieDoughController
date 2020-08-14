# CookieDoughController
A customizable game controller created using the Arduino Uno.

Uses the Arduino keyboard bootloader hex from Coopermaas library: https://github.com/coopermaa/USBKeyboard


 To configure the controller and run the program with a custom configuration:
1. Create an external config file ending with the extension ".cpp" in the src folder (if it is not already present).
2. Define a variable of type "Controller" named "controller".
3. Call the constructor of the "Controller" object with your prefered default and special function keymaps.
This will configure the controller to use your own custom configuration.

