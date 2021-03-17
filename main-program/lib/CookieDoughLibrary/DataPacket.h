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
    uint8_t *payload;
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
bool ReadDataPacketOnEEPROM(uint16_t adress, DataPacket *packet, uint16_t *packetSize);

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
bool SaveDataPacketToEEPROM(uint16_t adress, uint8_t *data, uint16_t dataSize, uint16_t *packetSize);

/**
 * @brief Joins the given data onto the end of the payload of a packet that is present on the EEPROM
 * and updates the packet information accordingly.
 * 
 * @param adress The adress to the stx of the packet.
 * @param data The data to be joined onto the end of the packets payload.
 * @param dataSize The size of the data to be joined.
 * @return true The data was successfully joined onto the packets payload.
 * @return false Unable to join the data with the packet.
 */
bool JoinDataWithDataPacketOnEEPROM(uint16_t adress, uint8_t *data, uint16_t dataSize);

/**
 * @brief Checks wether the DataPackets "active" flag is considered active or inactive.
 * 
 * @param activeFlag The value of the "active" flag of the DataPacket object.
 * @return true The DataPacket is active.
 * @return false The DataPacket is not active
 */
bool IsPacketActive(const uint8_t activeFlag);

/**
 * @brief Deactivates a packet present on the EEPROM.
 * 
 * @param adress The adress of the packet to be deactivated.
 * @return true The packet was successfully deactivated.
 * @return false The adress does not contain a packet.
 */
bool DeactivatePacket(uint16_t adress);

/**
 * @brief Attempts to find and return the adress of the first DataPacket that is found on the EEPROM.
 * 
 * @param startAdress The starting point of the search.
 * @param adressOfThePacket Outputs the adress of the packet that was found.
 * @param sizeOfThePacket Outputs the size of the packet that was found.
 * @param adressOfThePayload Outputs the adress of the payload on the found packet. 
 * @param lengthOfThePayload Outputs the length of the payload on the found packet.
 * @return true Successfully found a packet.
 * @return false Could not find a packet on the eeprom.
 */
bool FindFirstDataPacketOnEEPROM(uint16_t startAdress, uint16_t *adressOfThePacket, uint16_t *sizeOfThePacket, uint16_t *adressOfThePayload, uint16_t *lengthOfThePayload);

/**
 * @brief Deactivates all packets present on the EEPROM.
 * 
 * @return true Deactivated one or more packets
 * @return false Couldn't deactivate any packets.
 */
bool DeactivateAllPacketsOnEEPROM();

/**
 * @brief Prevents the provided adress from exceeding the last adress of the buffer,
 * and converts the adress into a cyclic format.
 * 
 * @param adress The adress to be made cyclic.
 * @param bufferSize The size of the cyclic buffer.
 * @return uint16_t Returns the adress in a safe cyclic format. If the adress exceeds
 * the last adress of the buffer, a new safe adress that wraps back to the start of the buffer
 * will be returned. 
 * If the adress does not exceed the buffers last adress the returned adress 
 * will be equal to the provided adress.
 * WARNING: When bufferSize is zero, the returned adress will always be zero.
 */
uint16_t CyclicAdress(uint32_t adress, uint16_t bufferSize);

/**
 * @brief Returns the amount of bytes that an empty DataPacket would occupy when serialized.
 * 
 * @return uint16_t Returns the amount of bytes an empty DataPacket would occupy when in a serialized form.
 */
uint16_t SizeOfEmptySerializedDataPacket();

/**
 * @brief Calculates the size in bytes that the given DataPacket will occupy when serialized.
 * 
 * @param packet The packet that you want to calculate the serialized size of.
 * @return uint16_t Returns the amount of bytes the packet will occupy when in a serialized form.
 */
uint16_t SizeOfSerializedDataPacket(const DataPacket &packet);

/**
 * @brief Read a series of bytes from the EEPROM and stores the result in the given array.
 * 
 * @param adress The adress to begin reading from the EEPROM.
 * @param amountOfBytes The amount of bytes we want to retrieve.
 * @param result An array of bytes to store the result.
 * @return true The series of bytes was successfully read from the EEPROM and stored into the result array.
 * @return false Could not read the series of bytes from the EEPROM.
 */
bool ReadBytesFromEEPROM(uint16_t adress, uint16_t amountOfBytes, uint8_t *result);

/**
 * @brief Checks whether the adress contains a valid packet or not.
 * 
 * @param adress The adress to be checked for a valid packet.
 * @return true The adress contains a valid packet.
 * @return false The adress does not contain a valid packet.
 */
bool IsPacketOnEEPROMValid(uint16_t adress);

/**
 * @brief Checks whether the adress contains a valid packet or not.
 * 
 * @param adress The adress to be checked for a valid packet.
 * @param sizeOfPacket Outputs the size of the packet.
 * @param adressOfPayload Outputs the adress of the payload.
 * @param lengthOfPayload Outputs the length of the payload.
 * @return true The adress contains a valid packet.
 * @return false The adress does not contain a valid packet.
 */
bool IsPacketOnEEPROMValid(uint16_t adress, uint16_t *sizeOfPacket, uint16_t *adressOfPayload, uint16_t *lengthOfPayload);

/**
 * @brief Calculates a CRC checksum for the given array of bytes (using the algorith CRC-32).
 * 
 * @param data The data we want to calculate a CRC checksum for.
 * @param length The size of the data in bytes.
 * (The length of the array of bytes).
 * @param crc If not provided, will calculate the crc using only the data 
 * provided. If provided, can be used to continue the calculation of a crc for a
 * stream of data. Pass in the crc from the precceding part of the stream
 * to continue the calculation of the crc. Do this until you reach the end of
 * the stream to calculate a checksum for the stream as a whole.
 * @return unsigned long Returns the CRC checksum calculated from the data (or data stream).
 */
uint32_t CalculateCRC(const uint8_t *data, uint16_t length, uint32_t crc = ~0L);

/**
 * @brief Used to constructs and sequentially write down a DataPacket to 
 * EEPROM.
 */
class DataPacketWriter
{
private:
    bool isCompleted; // Todo: Test that it is true after FinishPacket().
    bool success; // TODO: Test that this is set to false if any of the steps/methods fail.
    uint16_t address;
    uint16_t payloadLength; // TODO: Test that this represents the correct value after finishing the packet.
    uint32_t crc; // TODO: Test that this represents the correct value after finishing the packet.
    uint16_t packetSize; // TODO: Test that this represents the correct value after finishing the packet.
    uint16_t sizeOfEeprom;

public:

    /**
     * @brief Constructs a new object used for writing DataPackets to storage
     * in multiple steps. Will write the stx of the DataPacket being
     * constructed to storage on the given address.
     * 
     * @param packetAddress The storage address of the DataPacket being created, 
     * where the stx will be written.
     */
    DataPacketWriter(uint16_t packetAddress);
    DataPacketWriter(const DataPacketWriter& other) = delete;
    void operator=(const DataPacketWriter&) = delete;

    /**
     * @brief Adds the provided data into the payload of the DataPacket which
     * is being constructed on the storage. Can be used multiple times in a row
     * to push additional data to the end of the packets payload.
     * 
     * @param data The data to be added to the payload.
     * @param dataSize The size of the data to be added.
     * @return true The data has successfully been added to the payload.
     * @return false The data could not be added to the payload.
     */
    bool AddDataToPayload(const uint8_t *data, const uint16_t dataSize);

    /**
     * @brief Adds the provided data into the payload of the DataPacket which
     * is being constructed on the storage. Can be used multiple times in a row
     * to push additional data to the end of the packets payload.
     * 
     * @param data The data to be added to the payload.
     * @return true The data has successfully been added to the payload.
     * @return false The data could not be added to the payload.
     */
    template<class T>
    bool AddDataToPayload(const T &data)
    {
        return AddDataToPayload(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
    }

    // TODO: Document this...
    bool FinishWritingPacket(uint16_t *resultingPacketSize);

    // TODO: Write functionality (Not fully planned out):
    //  * 1 Constructor
    //      * Take in a packetAdress as argument.
    //      * Saves adress in object.
    //      * Set isCompleted to false.
    //      * Write stx to EEPROM on the given adress. (Take functionality and tests from 'SaveDataPacketToEEPROM')
    //      * If we succeed,
    //          * Set success to true.
    //      * If we fail to write stx to EEPROM on the adress,
    //          * Set success to false.
    //
    //  * 2 AddDataToPayload (writes payload step by step. Each call adds to payload)
    //      * if we are not successful, return false.
    //      * if isCompleted is already true, return false.
    //      * Test that the added data won't make the packet to big for the EEPROM.
    //      * ... Take functionality and tests from 'SaveDataPacketToEEPROM'.
    //      * Add data to the end of the payload.
    //      * Add size of data to the payloadLength on object.
    //      * Add size of data to the packetSize on object.
    //      * Add data to crc on object. (If it has not yet been set, start it off with the initial function)
    //      * If we fail to add the data to the payload,
    //          * Set success to false.
    //      * return success.
    //
    //  - 3 FinishWritingPacket (Make the user unable to call any other functions on this object after this step)
    //      - if we are not successful, return false.
    //      - if isCompleted is already true, return false.
    //      * Write active. (Cyclic address)
    //      * Write payloadLength. (Cyclic address)
    //      - Write crc. (Cyclic address)
    //      - Write etx. (Cyclic address)
    //      - (Take functionality and tests from 'SaveDataPacketToEEPROM')
    //      - if we fail to save packet to EEPROM,
    //          - Set success to false.
    //          - return false.
    //      - if we succeeded in writing the datapacket,
    //          - Set isCompleted to true.
    //          - Assign the out-variable "packetSize" the size of the packet.
    //          - Return success.


};

#endif
