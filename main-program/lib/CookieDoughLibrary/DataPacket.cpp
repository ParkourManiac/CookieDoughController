#include "DataPacket.h"
#include <stdlib.h>
#include <EEPROM.h>
#include <Arduino.h>

DataPacket::DataPacket()
    :payload(new uint8_t[1])
{
}

DataPacket::DataPacket(const uint8_t *data, const uint16_t dataSize)
    : payloadLength(dataSize)
    , payload(new uint8_t[this->payloadLength])
{
    for(int i = 0; i < payloadLength; i++) 
    {
        payload[i] = uint8_t(data[i]);
    }
    crc = CalculateCRC(payload, payloadLength);
}

DataPacket::DataPacket(const DataPacket &other) : DataPacket(other.payload, other.payloadLength)
{
    stx = other.stx;
    active = other.active;
    crc = other.crc;
    etx = other.etx;
}

DataPacket::~DataPacket()
{
    delete[](payload);
}

bool ParsePacketFromEEPROM(uint16_t adress, DataPacket *packet, uint16_t *packetSize)
{
    *packetSize = 0;
    unsigned int currentAdress = adress;

    // STX: If the first value is not equal to the stx...
    uint8_t stx = EEPROM.read(currentAdress);
    if (stx != packet->stx)
        return false;
    currentAdress += sizeof(packet->stx);

    EEPROM.get(currentAdress, packet->active);
    if (IsPacketActive(packet->active) == false) // TODO: TEST THIS IF STATEMENT
        return false;
    currentAdress += sizeof(packet->active);

    EEPROM.get(currentAdress, packet->payloadLength);
    // PAYLOAD LENGTH: If the payload is larger than what fits on the eeprom...
    if (packet->payloadLength > (EEPROM.length() - 8))
        return false;
    currentAdress += sizeof(packet->payloadLength);

    // CRC
    EEPROM.get(currentAdress, packet->crc);
    currentAdress += sizeof(packet->crc);

    // ETX: If the packet does not end with the etx...
    if (EEPROM.read(currentAdress + packet->payloadLength) != packet->etx)
        return false;

    // PAYLOAD: Retrieve payload as an uint8_t array.
    uint8_t *payloadFromEEPROM = new uint8_t[packet->payloadLength];
    for (unsigned int i = 0; i < packet->payloadLength; i++)
    {
        payloadFromEEPROM[i] = EEPROM.read(currentAdress + i);
    }
    currentAdress += packet->payloadLength * sizeof(packet->payload[0]);

    // Fill adress of packet->payload with the payload from eeprom.
    delete[](packet->payload);
    packet->payload = new uint8_t[packet->payloadLength];
    for (unsigned int i = 0; i < packet->payloadLength; i++)
    {
        packet->payload[i] = payloadFromEEPROM[i];
    }
    delete[](payloadFromEEPROM);

    // // DEBUG
    // DEBUG_PRINT(F("Reading: "));
    // for(int i = 0; i < packet->payloadLength; i++) {
    //     DEBUG_PRINT(packet->payload[i], HEX);
    // }
    // DEBUG_PRINT(F("\n"));
    // DEBUG(delay(100));
    // // DEBUG

    // If the crc of the payload is not equal to the crc of the packet...
    uint32_t payloadCRC = CalculateCRC(packet->payload, packet->payloadLength);
    if (packet->crc != payloadCRC)
        return false;

    // ETX: Move adress to end of packet and assign packet size.
    currentAdress += sizeof(packet->etx);
    *packetSize = static_cast<uint16_t>(currentAdress - adress);
    return true;
}

bool SavePacketToEEPROM(uint16_t adress, uint8_t *data, uint16_t dataSize, uint16_t *packetSize) 
{
    *packetSize = 0;
    uint16_t currentAdress = adress;

    // Create packet.
    DataPacket packet = DataPacket(data, dataSize);

    // Write packet.
    EEPROM.put(currentAdress, packet.stx);
    currentAdress = static_cast<uint16_t>(currentAdress + sizeof(packet.stx));

    EEPROM.put(currentAdress, packet.active);
    currentAdress = static_cast<uint16_t>(currentAdress + sizeof(packet.active));

    EEPROM.put(currentAdress, packet.payloadLength);
    currentAdress = static_cast<uint16_t>(currentAdress + sizeof(packet.payloadLength));
    EEPROM.put(currentAdress, packet.crc);
    currentAdress = static_cast<uint16_t>(currentAdress + sizeof(packet.crc));
    for (uint16_t i = 0; i < packet.payloadLength; i++)
    {
        EEPROM.update(currentAdress + i, packet.payload[i]);
    }
    currentAdress = static_cast<uint16_t>(currentAdress + (packet.payloadLength * sizeof(packet.payload[0])));
    EEPROM.put(currentAdress, packet.etx);
    currentAdress = static_cast<uint16_t>(currentAdress + sizeof(packet.etx));

    // // DEBUG
    // DEBUG_PRINT(F("Putting down: "));
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     DEBUG_PRINT(packet.payload[i], HEX);
    // }
    // DEBUG_PRINT(F("\n"));
    // DEBUG(delay(100));
    // // DEBUG

    // Verify that package can be read from memory correctly.
    DataPacket result;
    uint16_t _sizeOfPacket;
    bool success = ParsePacketFromEEPROM(adress, &result, &_sizeOfPacket);
    if (!success || packet.crc != result.crc)
    {
        return false; // Throw: Something went wrong when writing.
    } else
    {
        *packetSize = static_cast<uint16_t>(currentAdress - adress);
        DEBUG_PRINT(F("Size of packet: ")); // DEBUG
        DEBUG_PRINT(*packetSize); // DEBUG
        DEBUG_PRINT(F("\n"));
        DEBUG(delay(100)); // DEBUG
        return true; // Package saved successfully.
    }
}

bool IsPacketActive(const uint8_t activeFlag)
{
    return activeFlag == 0x01;
}

bool DeactivatePacket(uint16_t adress)
{
    uint16_t currentAdress = adress;
    DataPacket packetTemplate;
    uint16_t adressOfActiveFlag = static_cast<uint16_t>(adress + sizeof(packetTemplate.stx));
    uint8_t deactivatedFlag = 0x00;

    uint8_t stx = EEPROM.read(currentAdress);
    if(stx != packetTemplate.stx) 
    {
        return false;
    }
    currentAdress = static_cast<uint16_t>(currentAdress + sizeof(packetTemplate.stx));
    currentAdress = static_cast<uint16_t>(currentAdress + sizeof(packetTemplate.active));

    uint16_t payloadLength;
    EEPROM.get(currentAdress, payloadLength);
    currentAdress = static_cast<uint16_t>(
        currentAdress +
        sizeof(packetTemplate.payloadLength) +
        sizeof(packetTemplate.crc) + 
        sizeof(packetTemplate.payload[0]) * packetTemplate.payloadLength
    );

    uint8_t etx = EEPROM.read(currentAdress);
    if(etx != packetTemplate.etx)
    {
        return false;
    }

    EEPROM.put(adressOfActiveFlag, deactivatedFlag);
    return true;
}


uint32_t CalculateCRC(uint8_t *data, uint16_t length)
{
    const uint32_t crc_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c};

    uint32_t crc = ~0L;

    for (uint16_t index = 0; index < length; ++index)
    {
        crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
        crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
        crc = ~crc;
    }
    return crc;
}