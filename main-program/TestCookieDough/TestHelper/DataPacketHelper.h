#ifndef DATA_PACKET_HELPER_H
#define DATA_PACKET_HELPER_H

#include "../../lib/CookieDoughLibrary/DataPacket.h"

/**
 * @brief Sets up the behaviour of the mocked EEPROM functions so that
 * the ParsePacketFromEEPROM function will successfully return the provided DataPacket. 
 * 
 * 
 * @param expectedPacket The packet to be returned through the ParsePacketFromEEPROM function.
 * @param eepromSize The size of the eeprom. Note: Leaving this at 0 will let the function assign a eepromSize automatically that fits the provided packet.
 */
void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(const DataPacket &expectedPacket, uint16_t eepromSize = 0);

/**
 * @brief Calculates the expected size in bytes the packet will occupy on the EEPROM.
 * 
 * @param expectedPacket The packet to be used for calculating the size.
 */
uint16_t Helper_CalculateSizeOfPacketOnEEPROM(DataPacket expectedPacket);

#endif