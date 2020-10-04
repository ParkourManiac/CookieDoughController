#include "../test.h"
#include "DataPacketHelper.h"

extern std::vector<uint8_t> EEPROMClass_read_return_v;
extern std::vector<uint16_t> EEPROMClass_get_param_t_o1_vr;
extern std::vector<uint16_t> EEPROMClass_length_return_v;
extern std::vector<uint32_t > EEPROMClass_get_param_t_o2_vr;
extern std::vector<uint8_t > EEPROMClass_get_param_t_o3_vr;

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(const DataPacket &expectedPacket, uint16_t eepromSize)
{
    if(eepromSize == 0)
    {
        eepromSize = static_cast<uint16_t>(Helper_CalculateSizeOfPacketOnEEPROM(expectedPacket) + 100);
    }

    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o3_vr.push_back(expectedPacket.active);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_length_return_v.push_back(eepromSize);
    EEPROMClass_get_param_t_o2_vr.push_back(expectedPacket.crc);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);
    for (int i = 0; i < expectedPacket.payloadLength; i++) 
    {
        EEPROMClass_read_return_v.push_back(expectedPacket.payload[i]);
    }
}

void Helper_SavePacketToEEPROM_PrepareEepromSizeAndPrepareToReturnPacket(uint16_t adress, uint8_t *data, uint16_t dataSize, uint16_t eepromSize)
{
    DataPacket expectedPacket = DataPacket(data, dataSize);

    if(eepromSize == 0)
    {
        eepromSize = static_cast<uint16_t>(adress + Helper_CalculateSizeOfPacketOnEEPROM(expectedPacket) + 10);
    }

    EEPROMClass_length_return_v.push_back(eepromSize);
    Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(expectedPacket, eepromSize);
}

uint16_t Helper_CalculateSizeOfPacketOnEEPROM(DataPacket packet) 
{
    uint16_t payloadSize = packet.payloadLength * sizeof(packet.payload[0]);
    uint16_t sizeOfPacket = static_cast<uint16_t>(
        sizeof(packet.stx) + 
        sizeof(packet.active) + 
        sizeof(packet.payloadLength) + 
        sizeof(packet.crc) + 
        payloadSize + 
        sizeof(packet.etx)
    );
    return sizeOfPacket;
}

// TODO: Add adress to ParsePacket helper?
