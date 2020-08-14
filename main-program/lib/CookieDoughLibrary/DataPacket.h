#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <stdint.h>

/**
 * @brief A data packet used to safely read and write objects to memory.
 */
struct DataPacket
{
    uint8_t stx = 0x02;
    uint16_t payloadLength = 0;
    uint32_t crc = 0;
    uint8_t *payload = nullptr;
    uint8_t etx = 0x03;
};

/**
 * @brief Reads and parses a DataPacket from the eeprom.
 * @note The DataPacket packet reference must be allocated on the heap. 
 * otherwise this will cause an error.
 * 
 * @param adress The eeprom adress of the DataPacket. (The adress of the stx).
 * @param packet A DataPacket where the result will be stored. (MUST be allocated on the heap)
 * @param packetSize The complete size of the parsed DataPacket in bytes.
 * @return true When we successfully parsed the DataPacket from the eeprom.
 * @return false When the adress couldn't be parsed as a DataPacket.
 */
bool ParsePacketFromEEPROM(unsigned int adress, DataPacket &packet, unsigned int &packetSize);

/**
 * @brief Saves the given data as a DataPacket on the eeprom.
 * Note: The DataPacket is the wrapper for the data.
 * 
 * @param adress The eeprom adress where the DataPacket will be written to.
 * @param data The information we want to store (in the form of an array of bytes).
 * @param dataSize The size of the data in bytes.
 * @param packetSize The complete size of the DataPacket that was saved to the eeprom.
 * @return true When we successfully saved the DataPacket to the eeprom.
 * @return false When we were unsuccessful in saving the DataPacket to the eeprom.
 */
bool SavePacketToEEPROM(unsigned int adress, uint8_t *data, unsigned int dataSize, unsigned int &packetSize);

/**
 * @brief Calculates a CRC checksum for the given array of bytes (using the algorith CRC-32).
 * 
 * @param data The data we want to calculate a CRC checksum for.
 * @param length The size of the data in bytes.
 * (The length of the array of bytes).
 * @return unsigned long Returns the CRC checksum calculated from the data.
 */
unsigned long CalculateCRC(uint8_t *data, unsigned int length);


#endif