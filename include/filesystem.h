/**
 * @file filesystem.h
 * @brief Simple inode-based filesystem for HydraOS
 * @author Shubham Phapale
 * @date 2025
 */

#ifndef __HYDRAOS__FILESYSTEM_H
#define __HYDRAOS__FILESYSTEM_H

#include <common/types.h>
#include <drivers/ata.h>

namespace hydraos
{
    namespace filesystem
    {
        // File system constants
        #define FS_SECTOR_SIZE      512
        #define FS_MAX_FILENAME     32
        #define FS_MAX_FILES        64
        #define FS_SUPERBLOCK_SEC   1
        #define FS_INODE_START_SEC  2
        #define FS_DATA_START_SEC   10
        #define FS_MAGIC            0x48594452  // "HYDR" in hex
        
        // File types
        enum InodeType
        {
            INODE_TYPE_EMPTY = 0,
            INODE_TYPE_REGULAR = 1,
            INODE_TYPE_DIRECTORY = 2
        };
        
        // Inode structure (file metadata)
        struct Inode
        {
            char name[FS_MAX_FILENAME];
            hydraos::common::uint32_t size;
            hydraos::common::uint32_t startSector;
            hydraos::common::uint32_t type;
            hydraos::common::uint32_t created;
            hydraos::common::uint32_t modified;
        } __attribute__((packed));
        
        // Superblock structure (file system metadata)
        struct Superblock
        {
            hydraos::common::uint32_t magic;
            hydraos::common::uint32_t totalSectors;
            hydraos::common::uint32_t usedSectors;
            hydraos::common::uint32_t inodeCount;
            char label[32];
        } __attribute__((packed));
        
        class FileSystem
        {
        public:
            Inode inodes[FS_MAX_FILES];  // Public for shell access
            
        private:
            drivers::AdvancedTechnologyAttachment* disk;
            Superblock superblock;
            bool mounted;
            
            void LoadSuperblock();
            void SaveSuperblock();
            void LoadInodeTable();
            void SaveInodeTable();
            hydraos::common::int32_t FindFreeInode();
            hydraos::common::uint32_t AllocateSector();
            
        public:
            FileSystem(drivers::AdvancedTechnologyAttachment* diskDriver);
            ~FileSystem();
            
            // File system operations
            bool Format(const char* label);
            bool Mount();
            void Unmount();
            
            // File operations
            hydraos::common::int32_t CreateFile(const char* filename);
            hydraos::common::int32_t OpenFile(const char* filename);
            bool DeleteFile(const char* filename);
            hydraos::common::int32_t ReadFile(hydraos::common::int32_t inodeIndex, 
                                              hydraos::common::uint8_t* buffer, 
                                              hydraos::common::uint32_t size);
            hydraos::common::int32_t WriteFile(hydraos::common::int32_t inodeIndex, 
                                               const hydraos::common::uint8_t* buffer, 
                                               hydraos::common::uint32_t size);
            
            // Directory operations
            void ListFiles();
            bool FileExists(const char* filename);
            hydraos::common::uint32_t GetFileSize(const char* filename);
            
            // Info
            void PrintInfo();
        };
    }
}

#endif
