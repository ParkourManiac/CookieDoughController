#include "DataPacket.h"
#include <stdlib.h>
#include <EEPROM.h>
#include <Arduino.h>

// TODO: Needs to be tested.
/**
 * @brief Parses a DataPacket from the eeprom.
 * 
 * @param adress The eeprom adress of the DataPackets start position.
 * @param packet The DataPacket where the result will be stored.
 * @param packetSize The complete size of the parsed DataPacket in bytes.
 * @return true When we successfully parsed the DataPacket from the eeprom.
 * @return false When the adress couldn't be parsed as a DataPacket.
 */
bool ParsePacketFromEEPROM(unsigned int adress, DataPacket &packet, unsigned int &packetSize)
{
    packetSize = 0;
    unsigned int currentAdress = adress;

    // STX: If the first value is not equal to the stx...
    uint8_t stx = EEPROM.read(currentAdress);
    if (stx != packet.stx)
        return false;
    currentAdress += sizeof(packet.stx);

    EEPROM.get(currentAdress, packet.payloadLength);
    // PAYLOAD LENGTH: If the payload is larger than what fits on the eeprom...
    if (packet.payloadLength > (EEPROM.length() - 8))
        return false;
    currentAdress += sizeof(packet.payloadLength);

    // CRC
    EEPROM.get(currentAdress, packet.crc);
    currentAdress += sizeof(packet.crc);

    // ETX: If the packet does not end with the etx...
    if (EEPROM.read(currentAdress + packet.payloadLength) != packet.etx)
        return false;

    // PAYLOAD: Retrieve payload as an uint8_t array.
    uint8_t payload[packet.payloadLength];
    for (unsigned int i = 0; i < packet.payloadLength; i++)
    {
        payload[i] = EEPROM.read(currentAdress + i);
    }
    currentAdress += packet.payloadLength * sizeof(packet.payload[0]);

    // Fill adress of packet.payload with the payload from eeprom.
    unsigned int payloadByteSize = packet.payloadLength * sizeof(packet.payload[0]);
    packet.payload = (uint8_t*) realloc(packet.payload, payloadByteSize); // TODO: PREVENT THIS FROM BEING A MEMORY LEAK.
    for (unsigned int i = 0; i < packet.payloadLength; i++)
    {
        packet.payload[i] = payload[i];
    }

    // // DEBUG
    // Serial.print("Reading: ");
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     Serial.print(packet.payload[i], HEX);
    // }
    // Serial.println();
    // delay(100);
    // // DEBUG

    // If the crc of the payload is not equal to the crc of the packet...
    unsigned long payloadCRC = CalculateCRC(packet.payload, packet.payloadLength);
    if (packet.crc != payloadCRC)
        return false;

    // ETX: Move adress to end of packet and assign packet size.
    currentAdress += sizeof(packet.etx);
    packetSize = currentAdress - adress;
    return true;
}

// TODO: NEEDS TO BE TESTED
/**
 * @brief Saves the given data as a DataPacket on the eeprom.
 * Note: The DataPacket is the wrapper for the data.
 * 
 * @param adress The adress where we want to save the DataPacket.
 * @param data The information we want to store (in the form of an array of bytes).
 * @param dataSize The size of the data in bytes.
 * @param packetSize The complete size of the DataPacket that was saved to the eeprom.
 * @return true When we successfully saved the DataPacket to the eeprom.
 * @return false When we were unsuccessful in saving the DataPacket to the eeprom.
 */
bool SavePacketToEEPROM(unsigned int adress, uint8_t *data, unsigned int dataSize, unsigned int &packetSize) 
{
    packetSize = 0;
    unsigned int currentAdress = adress;

    // Create packet.
    DataPacket packet;
    packet.payload = data;
    packet.payloadLength = dataSize;
    packet.crc = CalculateCRC(packet.payload, packet.payloadLength);

    // Write packet.
    EEPROM.put(currentAdress, packet.stx);
    currentAdress += sizeof(packet.stx);
    EEPROM.put(currentAdress, packet.payloadLength);
    currentAdress += sizeof(packet.payloadLength);
    EEPROM.put(currentAdress, packet.crc);
    currentAdress += sizeof(packet.crc);
    for (unsigned int i = 0; i < packet.payloadLength; i++)
    {
        EEPROM.update(currentAdress + i, packet.payload[i]);
    }
    currentAdress += packet.payloadLength * sizeof(packet.payload[0]);
    EEPROM.put(currentAdress, packet.etx);
    currentAdress += sizeof(packet.etx);

    // // DEBUG
    // Serial.print("Putting down: ");
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     Serial.print(packet.payload[i], HEX);
    // }
    // Serial.println();
    // delay(100);
    // // DEBUG

    // Verify that package can be read from memory correctly.
    DataPacket *dataPtr = new DataPacket();
    DataPacket packetFromEeprom =  *dataPtr;
    unsigned int _sizeOfPacket;
    bool success = ParsePacketFromEEPROM(adress, packetFromEeprom, _sizeOfPacket);
    if (!success || packet.crc != packetFromEeprom.crc)
    {
        delete(dataPtr);
        return false; // Something went wrong when writing.
    }

    packetSize = currentAdress - adress;
    delete(dataPtr);
    return true; // Package saved successfully.
}

/**
 * @brief Calculates a CRC for the given array of bytes.
 * 
 * @param data The data we want to generate a CRC for.
 * @param length The size of the data in bytes.
 * (The length of the array of bytes).
 * @return unsigned long Returns the CRC generated from the data.
 */
unsigned long CalculateCRC(uint8_t *data, unsigned int length)
{

    const unsigned long crc_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c};

    unsigned long crc = ~0L;

    for (unsigned int index = 0; index < length; ++index)
    {
        crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
        crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
        crc = ~crc;
    }
    return crc;
}