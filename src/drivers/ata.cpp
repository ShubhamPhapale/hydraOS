#include <drivers/ata.h>

using namespace hydraos::common;
using namespace hydraos::drivers;
using namespace hydraos::hardwarecommunication;

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(bool master, uint16_t portBase)
:   dataPort(portBase),
    errorPort(portBase + 0x1),
    sectorCountPort(portBase + 0x2),
    lbaLowPort(portBase + 0x3),
    lbaMidPort(portBase + 0x4),
    lbaHiPort(portBase + 0x5),
    devicePort(portBase + 0x6),
    commandPort(portBase + 0x7),
    controlPort(portBase + 0x206)
{
    this->master = master;
    this->bytesPerSector = 512;
}

AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment()
{
}

void AdvancedTechnologyAttachment::Wait()
{
    // Wait 400ns by reading the status port 4 times
    for(int i = 0; i < 4; i++)
        commandPort.Read();
}

uint8_t AdvancedTechnologyAttachment::GetStatus()
{
    return commandPort.Read();
}

bool AdvancedTechnologyAttachment::WaitReady()
{
    uint8_t status = GetStatus();
    uint32_t timeout = 100000;
    
    // Wait while busy (with timeout)
    while((status & ATA_SR_BSY) && timeout > 0)
    {
        status = GetStatus();
        timeout--;
    }
    
    if(timeout == 0)
        return false;
    
    // Check for errors
    if(status & ATA_SR_ERR)
        return false;
    
    if(status & ATA_SR_DF)
        return false;
    
    return true;
}

bool AdvancedTechnologyAttachment::HasError()
{
    uint8_t status = GetStatus();
    return (status & ATA_SR_ERR) || (status & ATA_SR_DF);
}

void AdvancedTechnologyAttachment::Identify()
{
    // Select drive
    devicePort.Write(master ? 0xA0 : 0xB0);
    Wait();
    
    // Set sector count and LBA to 0
    sectorCountPort.Write(0);
    lbaLowPort.Write(0);
    lbaMidPort.Write(0);
    lbaHiPort.Write(0);
    
    // Send IDENTIFY command
    commandPort.Write(ATA_CMD_IDENTIFY);
    Wait();
    
    uint8_t status = GetStatus();
    
    // If status is 0, drive does not exist
    if(status == 0)
        return;
    
    // Wait until drive is ready
    if(!WaitReady())
        return;
    
    // Wait for DRQ (data request) bit to be set (with timeout)
    uint32_t timeout = 100000;
    status = GetStatus();
    while(!(status & ATA_SR_DRQ) && timeout > 0)
    {
        status = GetStatus();
        timeout--;
    }
    
    if(timeout == 0)
        return; // Timeout
    
    // Read 256 16-bit values (512 bytes)
    for(int i = 0; i < 256; i++)
    {
        uint16_t data = dataPort.Read();
        // We could parse the identification data here
        // For now, we just discard it
    }
}

void AdvancedTechnologyAttachment::Read28(uint32_t sectorNum, uint8_t* data, int count)
{
    if(sectorNum > 0x0FFFFFFF)
        return; // 28-bit LBA only supports up to 0x0FFFFFFF
    
    // Wait for drive to be ready
    if(!WaitReady())
        return;
    
    // Select drive and set highest 4 bits of LBA
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sectorNum >> 24) & 0x0F));
    Wait();
    
    // Error port (not used for read)
    errorPort.Write(0);
    
    // Sector count
    sectorCountPort.Write(count);
    
    // LBA (low, mid, high)
    lbaLowPort.Write(sectorNum & 0xFF);
    lbaMidPort.Write((sectorNum >> 8) & 0xFF);
    lbaHiPort.Write((sectorNum >> 16) & 0xFF);
    
    // Send READ command
    commandPort.Write(ATA_CMD_READ_PIO);
    
    // Read sectors
    for(int s = 0; s < count; s++)
    {
        // Wait for data to be ready (with timeout)
        uint32_t timeout = 100000;
        uint8_t status = GetStatus();
        while(!(status & ATA_SR_DRQ) && timeout > 0)
        {
            status = GetStatus();
            timeout--;
        }
        
        if(timeout == 0)
            return; // Timeout
        
        // Read 256 16-bit values (512 bytes)
        for(int i = 0; i < 256; i++)
        {
            uint16_t wdata = dataPort.Read();
            data[s * 512 + i * 2] = wdata & 0xFF;
            data[s * 512 + i * 2 + 1] = (wdata >> 8) & 0xFF;
        }
    }
}

void AdvancedTechnologyAttachment::Write28(uint32_t sectorNum, uint8_t* data, int count)
{
    if(sectorNum > 0x0FFFFFFF)
        return; // 28-bit LBA only supports up to 0x0FFFFFFF
    
    // Wait for drive to be ready
    if(!WaitReady())
        return;
    
    // Select drive and set highest 4 bits of LBA
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sectorNum >> 24) & 0x0F));
    Wait();
    
    // Error port (features for write)
    errorPort.Write(0);
    
    // Sector count
    sectorCountPort.Write(count);
    
    // LBA (low, mid, high)
    lbaLowPort.Write(sectorNum & 0xFF);
    lbaMidPort.Write((sectorNum >> 8) & 0xFF);
    lbaHiPort.Write((sectorNum >> 16) & 0xFF);
    
    // Send WRITE command
    commandPort.Write(ATA_CMD_WRITE_PIO);
    
    // Write sectors
    for(int s = 0; s < count; s++)
    {
        // Wait for data request (with timeout)
        uint32_t timeout = 100000;
        uint8_t status = GetStatus();
        while(!(status & ATA_SR_DRQ) && timeout > 0)
        {
            status = GetStatus();
            timeout--;
        }
        
        if(timeout == 0)
            return; // Timeout
        
        // Write 256 16-bit values (512 bytes)
        for(int i = 0; i < 256; i++)
        {
            uint16_t wdata = data[s * 512 + i * 2] | 
                            (((uint16_t)data[s * 512 + i * 2 + 1]) << 8);
            dataPort.Write(wdata);
        }
    }
    
    // Flush cache
    Flush();
}

void AdvancedTechnologyAttachment::Flush()
{
    // Select drive
    devicePort.Write(master ? 0xE0 : 0xF0);
    Wait();
    
    // Send CACHE FLUSH command
    commandPort.Write(ATA_CMD_CACHE_FLUSH);
    
    // Wait for completion
    WaitReady();
}
