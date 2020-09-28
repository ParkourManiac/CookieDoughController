#ifndef DATA_PACKET_HELPER_H
#define DATA_PACKET_HELPER_H

#include "../../lib/CookieDoughLibrary/DataPacket.h"

/**
 * @brief Sets up the behaviour of the mocked EEPROM functions so that
 * the ParsePacketFromEEPROM function will successfully return the provided DataPacket. 
 * 
 * 
 * @param expectedPacket The packet to be returned through the ParsePacketFromEEPROM function.
 */
void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(DataPacket expectedPacket);

/**
 * @brief Calculates the expected size in bytes the packet will occupy on the EEPROM.
 * 
 * @param expectedPacket The packet to be used for calculating the size.
 */
uint16_t Helper_CalculateSizeOfPacketOnEEPROM(DataPacket expectedPacket);

#endif