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
    , payload(new uint8_t[dataSize])
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

bool ReadDataPacketOnEEPROM(uint16_t adress, DataPacket *packet, uint16_t *packetSize)
{
    *packetSize = 0;
    uint32_t offset = 0;
    uint32_t currentAdress = 0;
    uint16_t sizeOfEeprom = EEPROM.length();

    if(adress >= sizeOfEeprom)
    {
        DEBUG_PRINT(F("ERROR: Tried to read packet from outside of the eeproms range. Adress was out of range."));
        return false;
    }

    // STX: If the first value is not equal to the stx...
    uint8_t stx = EEPROM.read(adress);
    if (stx != packet->stx)
        return false;
    offset += sizeof(packet->stx);

    currentAdress = (adress + offset) % sizeOfEeprom;
    EEPROM.get(currentAdress, packet->active);
    if (IsPacketActive(packet->active) == false) // TODO: TEST THIS IF STATEMENT
        return false;
    offset += sizeof(packet->active);

    // PAYLOAD LENGTH: 
    currentAdress = (adress + offset) % sizeOfEeprom;
    EEPROM.get(currentAdress, packet->payloadLength);
    uint16_t packetSizeExcludingPayload = static_cast<uint16_t>(
        sizeof(packet->stx) +
        sizeof(packet->active) +
        sizeof(packet->payloadLength) +
        sizeof(packet->crc) +
        sizeof(packet->etx)
    );
    // If the payload is larger than what fits on the eeprom...
    if (packet->payloadLength > (sizeOfEeprom - packetSizeExcludingPayload))
        return false;
    offset += sizeof(packet->payloadLength);


    // CRC
    currentAdress = (adress + offset) % sizeOfEeprom;
    EEPROM.get(currentAdress, packet->crc);
    offset += sizeof(packet->crc);

    // ETX: If the packet does not end with the etx...
    currentAdress = (adress + offset) % sizeOfEeprom;
    if (EEPROM.read(currentAdress + packet->payloadLength) != packet->etx)
        return false;

    // PAYLOAD: Retrieve payload as an uint8_t array.
    currentAdress = (adress + offset) % sizeOfEeprom;
    uint8_t *payloadFromEEPROM = new uint8_t[packet->payloadLength];
    for (unsigned int i = 0; i < packet->payloadLength; i++)
    {
        payloadFromEEPROM[i] = EEPROM.read(currentAdress + i);
    }
    offset += packet->payloadLength * sizeof(packet->payload[0]);


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

    // ETX: Move offset to end of packet and assign it to packet size.
    offset += sizeof(packet->etx);
    *packetSize = static_cast<uint16_t>(offset);
    return true;
}

bool SaveDataPacketToEEPROM(uint16_t adress, uint8_t *data, uint16_t dataSize, uint16_t *packetSize) 
{
    *packetSize = 0;
    uint32_t offset = 0;
    uint32_t currentAdress = 0;
    uint16_t sizeOfEeprom = EEPROM.length();

    if(adress >= sizeOfEeprom)
    {
        DEBUG_PRINT(F("ERROR: Tried to save the packet outside of the eeproms range. Adress out of range."));
        return false;
    }

    // Create packet.
    DataPacket packet = DataPacket(data, dataSize);

    // Write packet.
    EEPROM.put(adress, packet.stx);
    offset = sizeof(packet.stx);

    currentAdress = (adress + offset) % sizeOfEeprom;
    EEPROM.put(currentAdress, packet.active);
    offset += sizeof(packet.active);

    currentAdress = (adress + offset) % sizeOfEeprom;
    EEPROM.put(currentAdress, packet.payloadLength);
    offset += sizeof(packet.payloadLength);

    currentAdress = (adress + offset) % sizeOfEeprom;
    EEPROM.put(currentAdress, packet.crc);
    offset += sizeof(packet.crc);

    currentAdress = (adress + offset) % sizeOfEeprom;
    for (uint16_t i = 0; i < packet.payloadLength; i++)
    {
        EEPROM.update(currentAdress + i, packet.payload[i]);
    }
    offset += (packet.payloadLength * sizeof(packet.payload[0]));

    currentAdress = (adress + offset) % sizeOfEeprom;
    EEPROM.put(currentAdress, packet.etx);
    offset += sizeof(packet.etx);

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
    bool success = ReadDataPacketOnEEPROM(adress, &result, &_sizeOfPacket);
    if (!success || packet.crc != result.crc)
    {
        return false; // Throw: Something went wrong when writing.
    } else
    {
        *packetSize = static_cast<uint16_t>(offset);
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