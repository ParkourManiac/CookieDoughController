#include "DataPacket.h"
#include <stdlib.h>
#include <EEPROM.h>
#include <Arduino.h>

DataPacket::DataPacket()
    :payload(new uint8_t[1])
{
}

DataPacket::DataPacket(const uint8_t *data, const uint16_t dataSize)
    : payloadLength(dataSize)
    , payload(new uint8_t[dataSize])
{
    for(uint16_t i = 0; i < payloadLength; i++) 
    {
        payload[i] = uint8_t(data[i]);
    }
    crc = CalculateCRC(payload, payloadLength);
}

DataPacket::DataPacket(const DataPacket &other) : DataPacket(other.payload, other.payloadLength)
{
    stx = other.stx;
    active = other.active;
    crc = other.crc;
    etx = other.etx;
}

DataPacket::~DataPacket()
{
    delete[](payload);
}

bool ReadDataPacketOnEEPROM(uint16_t adress, DataPacket *packet, uint16_t *packetSize)
{
    *packetSize = 0;
    uint32_t offset = 0;
    uint32_t currentAdress = 0;
    uint16_t sizeOfEeprom = EEPROM.length();

    if(adress >= sizeOfEeprom)
    {
        DEBUG_PRINT(F("ERROR: Tried to read packet from outside of the eeproms range. Adress was out of range."));
        return false;
    }

    if(IsPacketOnEEPROMValid(adress) == false)
    {
        return false;
    }

    // STX
    offset += sizeof(packet->stx);

    // ACTIVE FLAG
    offset += sizeof(packet->active);

    // PAYLOAD LENGTH: 
    currentAdress = CyclicAdress(adress + offset, sizeOfEeprom);
    EEPROM.get(currentAdress, packet->payloadLength);
    offset += sizeof(packet->payloadLength);

    // CRC
    currentAdress = CyclicAdress(adress + offset, sizeOfEeprom);
    EEPROM.get(currentAdress, packet->crc);
    offset += sizeof(packet->crc);

    // PAYLOAD: Retrieve payload as an uint8_t array.
    currentAdress = CyclicAdress(adress + offset, sizeOfEeprom);
    uint8_t *payloadFromEEPROM = new uint8_t[packet->payloadLength];
    for (unsigned int i = 0; i < packet->payloadLength; i++)
    {
        uint16_t currentPayloadAdress = CyclicAdress(currentAdress + i, sizeOfEeprom);
        payloadFromEEPROM[i] = EEPROM.read(currentPayloadAdress);
    }
    offset += packet->payloadLength * sizeof(packet->payload[0]);


    // Fill adress of packet->payload with the payload from eeprom.
    delete[](packet->payload);
    packet->payload = new uint8_t[packet->payloadLength];
    for (unsigned int i = 0; i < packet->payloadLength; i++)
    {
        packet->payload[i] = payloadFromEEPROM[i];
    }
    delete[](payloadFromEEPROM);

    // // DEBUG
    // DEBUG_PRINT(F("Reading: "));
    // for(int i = 0; i < packet->payloadLength; i++) {
    //     DEBUG_PRINT(packet->payload[i], HEX);
    // }
    // DEBUG_PRINT(F("\n"));
    // DEBUG(delay(100));
    // // DEBUG

    // ETX: Move offset to end of packet and assign it to packet size.
    offset += sizeof(packet->etx);
    *packetSize = static_cast<uint16_t>(offset);
    return true;
}

bool SaveDataPacketToEEPROM(uint16_t adress, uint8_t *data, uint16_t dataSize, uint16_t *packetSize) 
{
    *packetSize = 0;
    uint32_t offset = 0;
    uint32_t currentAdress = 0;
    uint16_t sizeOfEeprom = EEPROM.length();

    if(adress >= sizeOfEeprom)
    {
        DEBUG_PRINT(F("ERROR: Tried to save the packet outside of the eeproms range. Adress out of range. \n"));
        return false;
    }

    // Create packet.
    DataPacket packet = DataPacket(data, dataSize);

    if(SizeOfSerializedDataPacket(packet) > sizeOfEeprom)
    {
        DEBUG_PRINT(F("ERROR: Tried to save a packet that was larger than the EEPROM. \n"));
        return false;
    }

    // Write packet.
    EEPROM.put(adress, packet.stx);
    offset = sizeof(packet.stx);

    currentAdress = CyclicAdress(adress + offset, sizeOfEeprom);
    EEPROM.put(currentAdress, packet.active);
    offset += sizeof(packet.active);

    currentAdress = CyclicAdress(adress + offset, sizeOfEeprom);
    EEPROM.put(currentAdress, packet.payloadLength);
    offset += sizeof(packet.payloadLength);

    currentAdress = CyclicAdress(adress + offset, sizeOfEeprom);
    EEPROM.put(currentAdress, packet.crc);
    offset += sizeof(packet.crc);

    currentAdress = CyclicAdress(adress + offset, sizeOfEeprom);
    for (uint16_t i = 0; i < packet.payloadLength; i++)
    {
        uint16_t currentPayloadAdress = CyclicAdress(currentAdress + i, sizeOfEeprom);
        EEPROM.update(currentPayloadAdress, packet.payload[i]);
    }
    offset += (packet.payloadLength * sizeof(packet.payload[0]));

    currentAdress = CyclicAdress(adress + offset, sizeOfEeprom);
    EEPROM.put(currentAdress, packet.etx);
    offset += sizeof(packet.etx);

    // // DEBUG
    // DEBUG_PRINT(F("Putting down: "));
    // for(int i = 0; i < packet.payloadLength; i++) {
    //     DEBUG_PRINT(packet.payload[i], HEX);
    // }
    // DEBUG_PRINT(F("\n"));
    // DEBUG(delay(100));
    // // DEBUG

    // Verify that package can be read from memory correctly.
    DataPacket result;
    uint16_t _sizeOfPacket;
    bool success = ReadDataPacketOnEEPROM(adress, &result, &_sizeOfPacket);
    if (!success || packet.crc != result.crc)
    {
        DEBUG_PRINT(F("ERROR: Failed to save DataPacket."));
        return false; // Throw: Something went wrong when writing.
    } else
    {
        *packetSize = static_cast<uint16_t>(offset);
        DEBUG_PRINT(F("Size of packet: ")); // DEBUG
        DEBUG_PRINT(*packetSize); // DEBUG
        DEBUG_PRINT(F("\n"));
        DEBUG(delay(100)); // DEBUG
        return true; // Package saved successfully.
    }
}

bool JoinDataWithDataPacketOnEEPROM(uint16_t adress, uint8_t *data, uint16_t dataSize)
{
    DataPacket packet;
    uint16_t eepromSize = EEPROM.length();

    if(adress >= eepromSize)
    {
        DEBUG_PRINT(F("ERROR: Tried to access a packet outside of the eeproms range. Adress out of range. \n"));
        return false;
    }

    uint8_t stx = EEPROM.read(adress);
    if(stx != packet.stx)
    {
        return false;
    }

    uint16_t payloadLengthAdress = CyclicAdress((
        adress +
        sizeof(packet.stx) +
        sizeof(packet.active)
    ), eepromSize);
    EEPROM.get(payloadLengthAdress, packet.payloadLength);

    uint16_t newPacketSize = static_cast<uint16_t>(
        SizeOfSerializedDataPacket(DataPacket()) +
        packet.payloadLength +
        dataSize
    );
    if(newPacketSize > eepromSize)
    {
        DEBUG_PRINT(F("ERROR: Joining the data with the DataPacket will cause the packets size to exceed the EEPROMs size. Joined packet is too large. \n"));
        return false;
    }

    uint16_t crcAdress = CyclicAdress((
        payloadLengthAdress +
        sizeof(packet.payloadLength)
    ), eepromSize);

    uint16_t etxAdress = CyclicAdress((
        crcAdress +
        sizeof(packet.crc) + 
        packet.payloadLength
    ), eepromSize);
    uint8_t etx = EEPROM.read(etxAdress);
    if(etx != packet.etx)
    {
        return false;
    }

    EEPROM.get(crcAdress, packet.crc);

    uint16_t newPayloadLength = static_cast<uint16_t>(packet.payloadLength + dataSize);
    EEPROM.put(payloadLengthAdress, newPayloadLength);

    uint32_t newCrc = CalculateCRC(data, dataSize, packet.crc);
    EEPROM.put(crcAdress, newCrc);

    uint16_t adressToJoinDataOn = CyclicAdress(etxAdress, eepromSize);
    for(uint16_t i = 0; i < dataSize; i++)
    {
        uint16_t currentDataAdress = CyclicAdress(adressToJoinDataOn + i, eepromSize);
        EEPROM.update(currentDataAdress, data[i]);
    }

    uint16_t newEtxAdress = CyclicAdress((
        adressToJoinDataOn +
        dataSize
    ), eepromSize);
    EEPROM.update(newEtxAdress, packet.etx);

    return true;
}

bool IsPacketActive(const uint8_t activeFlag)
{
    return activeFlag == 0x01;
}

bool DeactivatePacket(uint16_t adress)
{
    uint16_t eepromSize = EEPROM.length();
    if(adress >= eepromSize) 
    {
        DEBUG_PRINT(F("ERROR: Tried to deactivate a packet outside of the eeproms range. Adress out of range. \n"));
        return false;
    }

    uint16_t currentAdress = adress;
    DataPacket packetTemplate;
    uint16_t adressOfActiveFlag = CyclicAdress(
            (adress + sizeof(packetTemplate.stx)
        ), eepromSize
    );
    uint8_t deactivatedFlag = 0x00;

    uint8_t stx = EEPROM.read(adress);
    if(stx != packetTemplate.stx) 
    {
        return false;
    }
    currentAdress = CyclicAdress((
            currentAdress + 
            sizeof(packetTemplate.stx) + 
            sizeof(packetTemplate.active)
        ), eepromSize
    );
    EEPROM.get(currentAdress, packetTemplate.payloadLength);

    currentAdress = CyclicAdress((
            currentAdress +
            sizeof(packetTemplate.payloadLength) +
            sizeof(packetTemplate.crc) + 
            sizeof(packetTemplate.payload[0]) * packetTemplate.payloadLength
        ), eepromSize
    );
    uint8_t etx = EEPROM.read(currentAdress);
    if(etx != packetTemplate.etx)
    {
        return false;
    }

    EEPROM.put(adressOfActiveFlag, deactivatedFlag);
    return true;
}

bool FindFirstDataPacketOnEEPROM(uint16_t startAdress, uint16_t *adressOfThePacket, uint16_t *sizeOfThePacket, uint16_t *adressOfThePayload, uint16_t *lengthOfThePayload)
{
    uint16_t eepromSize = EEPROM.length();
    if(startAdress >= eepromSize)
    {
        DEBUG_PRINT(F("ERROR: Tried to start searching for a packet outside of the eeproms range. Adress out of range. \n"));
        return false;
    }

    uint16_t currentAdress = 0;
    for(uint16_t i = 0; i < eepromSize; i++)
    {
        currentAdress = CyclicAdress(startAdress + i, eepromSize);
        if(IsPacketOnEEPROMValid(currentAdress, sizeOfThePacket, adressOfThePayload, lengthOfThePayload))
        {
            *adressOfThePacket = currentAdress;
            return true;
        }
    }
    return false;
}

bool DeactivateAllPacketsOnEEPROM()
{
    bool hasDeactivatedAPacket = false;

    uint16_t startAdress = 0;
    uint16_t packetAdress, packetSize, payloadAdress, payloadLength;
    while(FindFirstDataPacketOnEEPROM(startAdress, &packetAdress, &packetSize, &payloadAdress, &payloadLength))
    {
        if(DeactivatePacket(packetAdress))
        {
            hasDeactivatedAPacket = true;
            startAdress = static_cast<uint16_t>((packetAdress + packetSize));
        }
    }
    
    return hasDeactivatedAPacket;
}

uint16_t CyclicAdress(uint32_t adress, uint16_t bufferSize)
{
    if(bufferSize == 0) 
    {
        DEBUG_PRINT(F("ERROR: bufferSize cannot be zero when converting adress to a cyclic adress. \n"));
        return 0;
    }
    return static_cast<uint16_t>(adress % bufferSize);
}

uint16_t SizeOfEmptySerializedDataPacket() // TODO: Is there a way to automatically calculate the size here? So that we do not need to type each members by hand?
{
    DataPacket packet; // TODO: Can we somehow remove this allocation?
    return static_cast<uint16_t>(
        sizeof(packet.stx) +
        sizeof(packet.active) +
        sizeof(packet.payloadLength) +
        sizeof(packet.crc) +
        sizeof(packet.etx)
    );
}

uint16_t SizeOfSerializedDataPacket(const DataPacket &packet)
{
    return static_cast<uint16_t>(
        SizeOfEmptySerializedDataPacket() +
        sizeof(packet.payload[0]) * packet.payloadLength
    );
}

bool ReadBytesFromEEPROM(uint16_t adress, uint16_t amountOfBytes, uint8_t *result)
{
    uint16_t eepromSize = EEPROM.length();
    if(adress >= eepromSize)
    {
        DEBUG_PRINT(F("ERROR: Tried to read a series of bytes outside of the EEPROMs range. Adress out of range. \n"));
        return false;
    }
    if(amountOfBytes == 0)
    {
        DEBUG_PRINT(F("ERROR: Cannot retrieve zero bytes. Requested a series of bytes of size zero. \n"));
        return false;
    }
    if(amountOfBytes > eepromSize)
    {
        DEBUG_PRINT(F("ERROR: Tried to retrieve a series of bytes that is larger than what fits on the EEPROM. Requested series of bytes is to large. \n"));
        return false;
    }
    
    for(uint16_t i = 0; i < amountOfBytes; i++)
    {
        result[i] = EEPROM.read(CyclicAdress(adress + i, eepromSize));
    }
    return true;
}

bool IsPacketOnEEPROMValid(uint16_t adress, uint16_t *sizeOfPacket, uint16_t *adressOfPayload, uint16_t *lengthOfPayload)
{
    uint16_t eepromSize = EEPROM.length();
    if(adress >= eepromSize)
    {
        DEBUG_PRINT(F("ERROR: Tried to validate a packet outside of the EEPROMs range. Adress out of range. \n"));
        return false;
    }

    *adressOfPayload = *lengthOfPayload = *sizeOfPacket = 0;
    uint32_t offset = 0;
    DataPacket packet;
    uint8_t stx = EEPROM.read(adress);
    if(stx != packet.stx)
    {
        return false;
    }
    offset += sizeof(stx);

    uint16_t activeAdress = CyclicAdress(adress + offset, eepromSize);
    EEPROM.get(activeAdress, packet.active);
    if(IsPacketActive(packet.active) == false)
    {
        return false;
    }
    offset += sizeof(packet.active);


    uint16_t payloadLenghtAdress = CyclicAdress(adress + offset, eepromSize);
    EEPROM.get(payloadLenghtAdress, packet.payloadLength);
    bool isPacketEmpty = packet.payloadLength == 0;
    bool isPacketTooLarge = packet.payloadLength > (eepromSize - SizeOfEmptySerializedDataPacket());
    if(isPacketEmpty || isPacketTooLarge)
    {
        return false;
    }
    offset += sizeof(packet.payloadLength);


    uint16_t crcAdress = CyclicAdress(adress + offset, eepromSize);
    offset += sizeof(packet.crc);
    uint16_t payloadAdress = CyclicAdress(adress + offset, eepromSize);
    offset += packet.payloadLength;

    uint16_t etxAdress = CyclicAdress(adress + offset, eepromSize);
    uint8_t etx = EEPROM.read(etxAdress);
    if(etx != packet.etx)
    {
        return false;
    }
    offset += sizeof(packet.etx);

    uint8_t firstPayloadPart = EEPROM.read(payloadAdress);
    uint32_t calculatedCrc = CalculateCRC(&firstPayloadPart, sizeof(firstPayloadPart));
    for (uint16_t i = 1; i < packet.payloadLength; i++)
    {
        uint16_t currentPayloadAdress = CyclicAdress(payloadAdress + i, eepromSize);
        uint8_t currentPayloadPart = EEPROM.read(currentPayloadAdress);
        calculatedCrc = CalculateCRC(&currentPayloadPart, sizeof(currentPayloadPart), calculatedCrc);
    }

    EEPROM.get(crcAdress, packet.crc);
    if(calculatedCrc != packet.crc)
    {
        return false;
    }

    *lengthOfPayload = packet.payloadLength;
    *adressOfPayload = payloadAdress;
    *sizeOfPacket = static_cast<uint16_t>(offset);
    return true;
}

bool IsPacketOnEEPROMValid(uint16_t adress)
{
    uint16_t adressOfPayload, lengthOfPayload, sizeOfPacket;
    return IsPacketOnEEPROMValid(adress, &sizeOfPacket, &adressOfPayload, &lengthOfPayload);
}

uint32_t CalculateCRC(uint8_t *data, uint16_t length, uint32_t crc)
{
    const uint32_t crc_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c};

    for (uint16_t index = 0; index < length; ++index)
    {
        crc = crc_table[(crc ^ data[index]) & 0x0f] ^ (crc >> 4);
        crc = crc_table[(crc ^ (data[index] >> 4)) & 0x0f] ^ (crc >> 4);
        crc = ~crc;
    }
    return crc;
}

DataPacketWriter::DataPacketWriter(uint16_t packetAddress)
{
    success = true;
    // uint32_t offset = 0;
    // uint32_t currentAdress = 0;
    uint16_t sizeOfEeprom = EEPROM.length();

    if(packetAddress >= sizeOfEeprom)
    {
        DEBUG_PRINT(F("ERROR: Tried to begin writing a packet outside of the eeproms range. Adress out of range. \n"));
        success = false;
        return;
    }

    // Create packet.
    DataPacket packet; // = DataPacket(data, dataSize);

    // if(SizeOfSerializedDataPacket(packet) > sizeOfEeprom)
    // {
    //     DEBUG_PRINT(F("ERROR: Tried to save a packet that was larger than the EEPROM. \n"));
    //     return false;
    // }

    // Write packet.
    EEPROM.put(packetAddress, packet.stx);
    // offset = sizeof(packet.stx);

    // TODO: Test that success is true after we put down the stx.



    
}

