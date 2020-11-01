#ifndef DATA_PACKET_HELPER_H
#define DATA_PACKET_HELPER_H

#include "../../lib/CookieDoughLibrary/DataPacket.h"

/**
 * @brief Sets up the behaviour of the mocked EEPROM functions so that
 * the ReadDataPacketOnEEPROM function will successfully return the provided DataPacket. 
 * 
 * 
 * @param adress The adress from which the packet will be read.
 * @param expectedPacket The packet to be returned through the ReadDataPacketOnEEPROM function.
 * @param eepromSize The size of the eeprom. Note: Leaving this at 0 will let the function assign a eepromSize automatically that fits the provided packet.
 */
void Helper_ReadDataPacketOnEEPROM_PrepareToReturnPacket(uint16_t adress, const DataPacket &expectedPacket, uint16_t eepromSize = 0);

/**
 * @brief Sets up the behaviour of the mocked EEPROM functions so that
 * the IsPacketValidOnEEPROM function will successfully read the provided 
 * DataPacket when reading from the EEPROM library. 
 * 
 * @param adress The adress from which the packet will be validated.
 * @param expectedPacket The packet to be read through the 
 * IsPacketValidOnEEPROM function.
 * @param eepromSize The size of the eeprom. Note: Leaving this at 0 will let 
 * the function assign a eepromSize automatically that fits the provided packet.
 */
void Helper_IsPacketValidOnEEPROM_PrepareToReadPacket(uint16_t adress, const DataPacket &expectedPacket, uint16_t eepromSize = 0);

/**
 * @brief Configures the behaviour of the mocked EEPROM functions to have
 * ReadBytesFromEEPROM successfully return the payload from the provided 
 * DataPacket. 
 * 
 * @param packetAdress The adress of the packet.
 * @param expectedPacket The packet containing the payload to be returned
 * through ReadBytesFromEEPROM.
 * @param eepromSize The size of the eeprom. Note: Leaving this at 0 will let 
 * the function assign a eepromSize automatically that fits the provided packet.
 */
void Helper_ReadBytesFromEEPROM_PreparesToReadPayload(uint16_t packetAdress, const DataPacket &expectedPacket, uint16_t eepromSize = 0);

/**
 * @brief Sets up the behaviour of the mocked EEPROM functions so that
 * the SaveDataPacketToEEPROM function will return true. 
 * 
 * @param adress The adress where the packet will be saved.
 * @param data The data that will be saved.
 * @param dataSize The size of the data that will be saved.
 * @param eepromSize The size of the eeprom. Note: Leaving this at 0 will let the function assign a eepromSize automatically that fits the provided packet.
 */
void Helper_SaveDataPacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(uint16_t adress, uint8_t *data, uint16_t dataSize, uint16_t eepromSize = 0);

/**
 * @brief Calculates the expected size in bytes the packet will occupy on the EEPROM.
 * 
 * @param expectedPacket The packet to be used for calculating the size.
 */
uint16_t Helper_CalculateSizeOfPacketOnEEPROM(DataPacket expectedPacket);

#endif