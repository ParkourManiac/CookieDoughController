#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <stdint.h>

/**
 * @brief A data packet used to safely read and write objects to memory.
 */
struct DataPacket
{
    uint8_t stx = 0x02;
    uint16_t payloadLength;
    uint32_t crc;
    uint8_t *payload;
    uint8_t etx = 0x03;
};

/**
 * @brief Reads and parses a data packet from the eeprom memory.
 * 
 * @param adress The starting adress of the packet.
 * @param packet The output data packet.
 * @param packetSize The total size of the packet.
 * @return true If we successfully read the data packet from memory.
 * @return false If we were unsuccessful in reading the data
 * packet from memory.
 */
bool ParsePacketFromEEPROM(unsigned int adress, DataPacket &packet, unsigned int &packetSize);

/**
 * @brief Saves a data packet to memory.
 * 
 * @param adress The EEPROM adress where the packet will be written to.
 * @param data The data/payload to be saved.
 * @param dataSize The size of the data/payload in bytes.
 * @param packetSize The total size of the data packet.
 * @return true If we successfully wrote the data packet to memory.
 * @return false If we were unsuccessful in writing the data packet to memory
 */
bool SavePacketToEEPROM(unsigned int adress, uint8_t *data, unsigned int dataSize, unsigned int &packetSize);

/**
 * @brief Calculates a CRC checksum from the provided data.
 * NOTE: This is a modified version of the CRC function 
 * at arduino page: https://www.arduino.cc/en/Tutorial/EEPROMCrc
 * 
 * @param data The data that will be used to calculate the CRC checksum.
 * @param length The length of the data in bytes.
 * @return unsigned long Returns a CRC checksum.
 */
unsigned long CalculateCRC(uint8_t *data, unsigned int length);


#endif