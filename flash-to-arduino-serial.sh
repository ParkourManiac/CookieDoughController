#!/bin/bash
dfu-programmer atmega16u2 erase
dfu-programmer atmega16u2 flash Arduino-usbserial-uno.hex
dfu-programmer atmega16u2 reset

