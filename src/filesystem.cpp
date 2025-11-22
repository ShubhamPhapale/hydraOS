/**
 * @file filesystem.cpp
 * @brief Simple inode-based filesystem implementation for HydraOS
 * @author Shubham Phapale
 * @date 2025
 */

#include <filesystem.h>
#include <memory.h>

using namespace hydraos;
using namespace hydraos::common;
using namespace hydraos::filesystem;
using namespace hydraos::drivers;

#define FS_MAGIC 0x48594452  // "HYDR" in hex

FileSystem::FileSystem(AdvancedTechnologyAttachment* diskDriver)
{
    this->disk = diskDriver;
    this->mounted = false;
    
    // Initialize inode table
    for(int i = 0; i < FS_MAX_FILES; i++)
    {
        inodes[i].type = INODE_TYPE_EMPTY;
        inodes[i].size = 0;
    }
}

FileSystem::~FileSystem()
{
    if(mounted)
        Unmount();
}

void FileSystem::LoadSuperblock()
{
    uint8_t* buffer = (uint8_t*)malloc(512);
    if(buffer != 0)
    {
        disk->Read28(FS_SUPERBLOCK_SEC, buffer, 1);
        
        // Copy superblock data
        for(int i = 0; i < sizeof(Superblock); i++)
            ((uint8_t*)&superblock)[i] = buffer[i];
        
        free(buffer);
    }
}

void FileSystem::SaveSuperblock()
{
    uint8_t* buffer = (uint8_t*)malloc(512);
    if(buffer != 0)
    {
        // Clear buffer
        for(int i = 0; i < 512; i++)
            buffer[i] = 0;
        
        // Copy superblock data
        for(int i = 0; i < sizeof(Superblock); i++)
            buffer[i] = ((uint8_t*)&superblock)[i];
        
        disk->Write28(FS_SUPERBLOCK_SEC, buffer, 1);
        free(buffer);
    }
}

void FileSystem::LoadInodeTable()
{
    uint8_t* buffer = (uint8_t*)malloc(512);
    if(buffer != 0)
    {
        // Load inode sectors (8 sectors for 64 inodes)
        for(int sector = 0; sector < 8; sector++)
        {
            disk->Read28(FS_INODE_START_SEC + sector, buffer, 1);
            
            // Copy 8 inodes per sector
            for(int i = 0; i < 8; i++)
            {
                int inodeIndex = sector * 8 + i;
                if(inodeIndex < FS_MAX_FILES)
                {
                    for(int j = 0; j < sizeof(Inode); j++)
                        ((uint8_t*)&inodes[inodeIndex])[j] = buffer[i * 64 + j];
                }
            }
        }
        free(buffer);
    }
}

void FileSystem::SaveInodeTable()
{
    uint8_t* buffer = (uint8_t*)malloc(512);
    if(buffer != 0)
    {
        // Save inode sectors (8 sectors for 64 inodes)
        for(int sector = 0; sector < 8; sector++)
        {
            // Clear buffer
            for(int i = 0; i < 512; i++)
                buffer[i] = 0;
            
            // Copy 8 inodes per sector
            for(int i = 0; i < 8; i++)
            {
                int inodeIndex = sector * 8 + i;
                if(inodeIndex < FS_MAX_FILES)
                {
                    for(int j = 0; j < sizeof(Inode); j++)
                        buffer[i * 64 + j] = ((uint8_t*)&inodes[inodeIndex])[j];
                }
            }
            
            disk->Write28(FS_INODE_START_SEC + sector, buffer, 1);
        }
        free(buffer);
    }
}

int32_t FileSystem::FindFreeInode()
{
    for(int i = 0; i < FS_MAX_FILES; i++)
    {
        if(inodes[i].type == INODE_TYPE_EMPTY)
            return i;
    }
    return -1;
}

uint32_t FileSystem::AllocateSector()
{
    uint32_t sector = FS_DATA_START_SEC + superblock.usedSectors;
    superblock.usedSectors++;
    SaveSuperblock();
    return sector;
}

bool FileSystem::Format(const char* label)
{
    // Initialize superblock
    superblock.magic = FS_MAGIC;
    superblock.totalSectors = 1000;  // Reserve 1000 sectors
    superblock.usedSectors = 0;
    superblock.inodeCount = 0;
    
    // Copy label
    int i;
    for(i = 0; i < 31 && label[i] != '\0'; i++)
        superblock.label[i] = label[i];
    superblock.label[i] = '\0';
    
    // Initialize inode table
    for(i = 0; i < FS_MAX_FILES; i++)
    {
        inodes[i].type = INODE_TYPE_EMPTY;
        inodes[i].size = 0;
        inodes[i].startSector = 0;
        for(int j = 0; j < FS_MAX_FILENAME; j++)
            inodes[i].name[j] = '\0';
    }
    
    // Write to disk
    SaveSuperblock();
    SaveInodeTable();
    
    return true;
}

bool FileSystem::Mount()
{
    LoadSuperblock();
    
    // Check magic number
    if(superblock.magic != FS_MAGIC)
        return false;
    
    LoadInodeTable();
    mounted = true;
    return true;
}

void FileSystem::Unmount()
{
    if(mounted)
    {
        SaveSuperblock();
        SaveInodeTable();
        mounted = false;
    }
}

int32_t FileSystem::CreateFile(const char* filename)
{
    if(!mounted)
        return -1;
    
    // Check if file already exists
    if(FileExists(filename))
        return -1;
    
    // Find free inode
    int32_t inodeIndex = FindFreeInode();
    if(inodeIndex < 0)
        return -1;
    
    // Initialize inode
    Inode* inode = &inodes[inodeIndex];
    
    int i;
    for(i = 0; i < FS_MAX_FILENAME - 1 && filename[i] != '\0'; i++)
        inode->name[i] = filename[i];
    inode->name[i] = '\0';
    
    inode->type = INODE_TYPE_REGULAR;
    inode->size = 0;
    inode->startSector = 0;  // Will be allocated on first write
    inode->created = 0;      // Would use timer here
    inode->modified = 0;
    
    superblock.inodeCount++;
    
    SaveInodeTable();
    SaveSuperblock();
    
    return inodeIndex;
}

int32_t FileSystem::OpenFile(const char* filename)
{
    if(!mounted)
        return -1;
    
    for(int i = 0; i < FS_MAX_FILES; i++)
    {
        if(inodes[i].type != INODE_TYPE_EMPTY)
        {
            // Compare filename
            bool match = true;
            for(int j = 0; j < FS_MAX_FILENAME; j++)
            {
                if(inodes[i].name[j] != filename[j])
                {
                    match = false;
                    break;
                }
                if(filename[j] == '\0')
                    break;
            }
            
            if(match)
                return i;
        }
    }
    
    return -1;
}

bool FileSystem::DeleteFile(const char* filename)
{
    if(!mounted)
        return false;
    
    int32_t inodeIndex = OpenFile(filename);
    if(inodeIndex < 0)
        return false;
    
    // Mark inode as empty
    inodes[inodeIndex].type = INODE_TYPE_EMPTY;
    inodes[inodeIndex].size = 0;
    superblock.inodeCount--;
    
    SaveInodeTable();
    SaveSuperblock();
    
    return true;
}

int32_t FileSystem::ReadFile(int32_t inodeIndex, uint8_t* buffer, uint32_t size)
{
    if(!mounted || inodeIndex < 0 || inodeIndex >= FS_MAX_FILES)
        return -1;
    
    Inode* inode = &inodes[inodeIndex];
    if(inode->type == INODE_TYPE_EMPTY)
        return -1;
    
    // Read data from disk
    if(inode->size > 0 && inode->startSector > 0)
    {
        uint32_t sectorsToRead = (inode->size + 511) / 512;
        uint8_t* diskBuffer = (uint8_t*)malloc(512);
        
        if(diskBuffer != 0)
        {
            uint32_t bytesRead = 0;
            for(uint32_t i = 0; i < sectorsToRead && bytesRead < size; i++)
            {
                disk->Read28(inode->startSector + i, diskBuffer, 1);
                
                uint32_t bytesToCopy = 512;
                if(bytesRead + bytesToCopy > inode->size)
                    bytesToCopy = inode->size - bytesRead;
                if(bytesRead + bytesToCopy > size)
                    bytesToCopy = size - bytesRead;
                
                for(uint32_t j = 0; j < bytesToCopy; j++)
                    buffer[bytesRead + j] = diskBuffer[j];
                
                bytesRead += bytesToCopy;
            }
            
            free(diskBuffer);
            return bytesRead;
        }
    }
    
    return 0;
}

int32_t FileSystem::WriteFile(int32_t inodeIndex, const uint8_t* buffer, uint32_t size)
{
    if(!mounted || inodeIndex < 0 || inodeIndex >= FS_MAX_FILES)
        return -1;
    
    Inode* inode = &inodes[inodeIndex];
    if(inode->type == INODE_TYPE_EMPTY)
        return -1;
    
    // Allocate sectors if needed
    if(inode->startSector == 0)
    {
        inode->startSector = AllocateSector();
    }
    
    // Write data to disk
    uint32_t sectorsToWrite = (size + 511) / 512;
    uint8_t* diskBuffer = (uint8_t*)malloc(512);
    
    if(diskBuffer != 0)
    {
        uint32_t bytesWritten = 0;
        for(uint32_t i = 0; i < sectorsToWrite; i++)
        {
            // Clear buffer
            for(int j = 0; j < 512; j++)
                diskBuffer[j] = 0;
            
            // Copy data
            uint32_t bytesToCopy = 512;
            if(bytesWritten + bytesToCopy > size)
                bytesToCopy = size - bytesWritten;
            
            for(uint32_t j = 0; j < bytesToCopy; j++)
                diskBuffer[j] = buffer[bytesWritten + j];
            
            // Allocate additional sectors if needed
            if(i > 0)
                AllocateSector();
            
            disk->Write28(inode->startSector + i, diskBuffer, 1);
            bytesWritten += bytesToCopy;
        }
        
        free(diskBuffer);
        
        inode->size = size;
        inode->modified = 0;  // Would use timer here
        
        SaveInodeTable();
        return bytesWritten;
    }
    
    return -1;
}

void FileSystem::ListFiles()
{
    // This will be called from shell, so we can't print directly
    // The shell will handle the display
}

bool FileSystem::FileExists(const char* filename)
{
    return OpenFile(filename) >= 0;
}

uint32_t FileSystem::GetFileSize(const char* filename)
{
    int32_t inodeIndex = OpenFile(filename);
    if(inodeIndex >= 0)
        return inodes[inodeIndex].size;
    return 0;
}

void FileSystem::PrintInfo()
{
    // This will be called from shell
}
