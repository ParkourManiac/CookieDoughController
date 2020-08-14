#include "../test.h"
#include "DataPacketHelper.h"

extern std::vector<uint8_t> EEPROMClass_read_return_v;
extern std::vector<uint16_t> EEPROMClass_get_param_t_o1_vr;
extern std::vector<uint16_t> EEPROMClass_length_return_v;
extern std::vector<uint32_t > EEPROMClass_get_param_t_o2_vr;

void Helper_ParsePacketFromEEPROM_PrepareToReturnPacket(DataPacket expectedPacket)
{
    EEPROMClass_read_return_v.push_back(expectedPacket.stx);
    EEPROMClass_get_param_t_o1_vr.push_back(expectedPacket.payloadLength);
    EEPROMClass_length_return_v.push_back(expectedPacket.payloadLength + 100);
    EEPROMClass_get_param_t_o2_vr.push_back(expectedPacket.crc);
    EEPROMClass_read_return_v.push_back(expectedPacket.etx);
    for (int i = 0; i < expectedPacket.payloadLength; i++) 
    {
        EEPROMClass_read_return_v.push_back(expectedPacket.payload[i]);
    }
}