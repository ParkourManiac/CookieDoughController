#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <stdint.h>

/**
 * @brief A data packet used to safely read and write objects to memory.
 */
struct DataPacket
{
    uint8_t stx = 0x02;
    uint8_t active = 0x01;
    uint16_t payloadLength = 0;
    uint32_t crc = 0;
    uint8_t *payload; // Todo: Allocate array of one uint8_t in constructor. Deallocate in destructor.
    uint8_t etx = 0x03;

    DataPacket();
    DataPacket(const uint8_t *_payload, const uint16_t _payloadLength);
    DataPacket(const DataPacket &other);
    void operator=(const DataPacket &other) = delete;
    ~DataPacket();
};

/**
 * @brief Converts any type of data into a DataPacket.
 * 
 * @tparam T The type of the data passed into the function.
 * @param data The data to be converted into a payload.
 * @return DataPacket The data represented as a DataPacket.
 */
template<class T>
DataPacket DataToPacket(const T &data)
{
    return DataPacket(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
}

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
bool ParsePacketFromEEPROM(uint16_t adress, DataPacket *packet, uint16_t *packetSize);

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
bool SavePacketToEEPROM(uint16_t adress, uint8_t *data, uint16_t dataSize, uint16_t *packetSize);

/**
 * @brief Checks wether the DataPackets "active" flag is considered active or inactive.
 * 
 * @param activeFlag The value of the "active" flag of the DataPacket object.
 * @return true The DataPacket is active.
 * @return false The DataPacket is not active
 */
bool IsPacketActive(const uint8_t activeFlag);

/**
 * @brief Calculates a CRC checksum for the given array of bytes (using the algorith CRC-32).
 * 
 * @param data The data we want to calculate a CRC checksum for.
 * @param length The size of the data in bytes.
 * (The length of the array of bytes).
 * @return unsigned long Returns the CRC checksum calculated from the data.
 */
uint32_t CalculateCRC(uint8_t *data, uint16_t length);


#endif