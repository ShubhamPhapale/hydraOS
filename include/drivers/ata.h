#ifndef __HYDRAOS__DRIVERS__ATA_H
#define __HYDRAOS__DRIVERS__ATA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>

namespace hydraos
{
    namespace drivers
    {
        // ATA Command codes
        #define ATA_CMD_READ_PIO          0x20
        #define ATA_CMD_READ_PIO_EXT      0x24
        #define ATA_CMD_READ_DMA          0xC8
        #define ATA_CMD_READ_DMA_EXT      0x25
        #define ATA_CMD_WRITE_PIO         0x30
        #define ATA_CMD_WRITE_PIO_EXT     0x34
        #define ATA_CMD_WRITE_DMA         0xCA
        #define ATA_CMD_WRITE_DMA_EXT     0x35
        #define ATA_CMD_CACHE_FLUSH       0xE7
        #define ATA_CMD_CACHE_FLUSH_EXT   0xEA
        #define ATA_CMD_PACKET            0xA0
        #define ATA_CMD_IDENTIFY_PACKET   0xA1
        #define ATA_CMD_IDENTIFY          0xEC

        // ATA Status Register bits
        #define ATA_SR_BSY     0x80    // Busy
        #define ATA_SR_DRDY    0x40    // Drive ready
        #define ATA_SR_DF      0x20    // Drive write fault
        #define ATA_SR_DSC     0x10    // Drive seek complete
        #define ATA_SR_DRQ     0x08    // Data request ready
        #define ATA_SR_CORR    0x04    // Corrected data
        #define ATA_SR_IDX     0x02    // Index
        #define ATA_SR_ERR     0x01    // Error

        // ATA Error Register bits
        #define ATA_ER_BBK      0x80    // Bad block
        #define ATA_ER_UNC      0x40    // Uncorrectable data
        #define ATA_ER_MC       0x20    // Media changed
        #define ATA_ER_IDNF     0x10    // ID mark not found
        #define ATA_ER_MCR      0x08    // Media change request
        #define ATA_ER_ABRT     0x04    // Command aborted
        #define ATA_ER_TK0NF    0x02    // Track 0 not found
        #define ATA_ER_AMNF     0x01    // No address mark

        class AdvancedTechnologyAttachment
        {
        protected:
            hardwarecommunication::Port16Bit dataPort;
            hardwarecommunication::Port8Bit errorPort;
            hardwarecommunication::Port8Bit sectorCountPort;
            hardwarecommunication::Port8Bit lbaLowPort;
            hardwarecommunication::Port8Bit lbaMidPort;
            hardwarecommunication::Port8Bit lbaHiPort;
            hardwarecommunication::Port8Bit devicePort;
            hardwarecommunication::Port8Bit commandPort;
            hardwarecommunication::Port8Bit controlPort;
            
            bool master;
            hydraos::common::uint16_t bytesPerSector;
            
        public:
            AdvancedTechnologyAttachment(bool master, hydraos::common::uint16_t portBase);
            ~AdvancedTechnologyAttachment();
            
            void Identify();
            void Read28(hydraos::common::uint32_t sectorNum, hydraos::common::uint8_t* data, int count = 1);
            void Write28(hydraos::common::uint32_t sectorNum, hydraos::common::uint8_t* data, int count = 1);
            void Flush();
            
        protected:
            void Wait();
            bool WaitReady();
            hydraos::common::uint8_t GetStatus();
            bool HasError();
        };
    }
}

#endif
