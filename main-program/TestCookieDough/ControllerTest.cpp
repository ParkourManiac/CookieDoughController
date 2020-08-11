#include "test.h"
#include "../lib/CookieDoughLibrary/Controller.h"
#include "../lib/CookieDoughLibrary/DataPacket.h"
#include "Fakes/Arduino.h"
#include "Fakes/EEPROM.h"
#include "TestHelper/DataPacketHelper.h"

const int normalKeyCount = 4;
Key defaultKeyMap[normalKeyCount] = {
    // Key map Arrow keys
    Key(2, 80),
    Key(3, 82),
    Key(4, 81),
    Key(5, 79),
};

const int specialKeyCount = 3;
SpecialKey specialKeys[specialKeyCount] = {
    SpecialKey(10, toggleEditMode),
    SpecialKey(11, cycleKeyMap),
    SpecialKey(12, toggleDefaultKeyMap),
};

void LoadKeyMapsFromMemory_CorrectlyLoadsKeymapIntoList() {
    // TODO:  
    // * Refactor Controller to only use Class variables.
    // - Setup info to be retrieved through Parse packet.
    // - Create test for returning false if the pin numbers retrieved are not present in the default keymap.
    Controller controller(defaultKeyMap, normalKeyCount, specialKeys, specialKeyCount);
    uint16_t data = 1337;
    uint8_t *dataPtr = (uint8_t*)&data;
    DataPacket packet;
    packet.payloadLength = sizeof(data);
    packet.payload = dataPtr;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);

    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(packet);

    ASSERT_TEST(false);
}