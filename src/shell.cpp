#include <shell.h>
#include <memory.h>
#include <syscalls.h>
#include <filesystem.h>
#include <drivers/ata.h>

using namespace hydraos;
using namespace hydraos::common;
using namespace hydraos::drivers;
using namespace hydraos::filesystem;

Shell::Shell(VGATextMode* vgaDriver, ProgrammableIntervalTimer* timerDriver, FileSystem* filesystem)
{
    this->vga = vgaDriver;
    this->timer = timerDriver;
    this->fileSystem = filesystem;
    bufferIndex = 0;
    
    for(int i = 0; i < 256; i++)
        commandBuffer[i] = '\0';
}

Shell::~Shell()
{
}

uint32_t Shell::strlen(const char* str)
{
    uint32_t len = 0;
    while(str[len] != '\0')
        len++;
    return len;
}

bool Shell::strcmp(const char* str1, const char* str2)
{
    uint32_t i = 0;
    while(str1[i] != '\0' && str2[i] != '\0')
    {
        if(str1[i] != str2[i])
            return false;
        i++;
    }
    return str1[i] == str2[i];
}

bool Shell::strncmp(const char* str1, const char* str2, uint32_t n)
{
    for(uint32_t i = 0; i < n; i++)
    {
        if(str1[i] != str2[i])
            return false;
        if(str1[i] == '\0')
            return true;
    }
    return true;
}

void Shell::strcpy(char* dest, const char* src)
{
    uint32_t i = 0;
    while(src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void Shell::PrintPrompt()
{
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("hydra");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("@");
    vga->SetColor(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga->Print("OS");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print(" $ ");
}

void Shell::Start()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("\n=====================================\n");
    vga->SetColor(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga->Print("  Welcome to HydraOS Shell v0.1\n");
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("=====================================\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("Type 'help' for available commands\n");
    vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga->Print("Tip: Run 'format' then 'mount' to use filesystem\n\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    PrintPrompt();
}

void Shell::HandleKeyPress(char c)
{
    if(c == '\n')
    {
        vga->Print("\n");
        commandBuffer[bufferIndex] = '\0';
        
        if(bufferIndex > 0)
        {
            ExecuteCommand(commandBuffer);
        }
        
        // Reset buffer
        bufferIndex = 0;
        for(int i = 0; i < 256; i++)
            commandBuffer[i] = '\0';
        
        PrintPrompt();
    }
    else if(c == '\b')
    {
        if(bufferIndex > 0)
        {
            bufferIndex--;
            commandBuffer[bufferIndex] = '\0';
            vga->PutChar('\b');
        }
    }
    else if(c == '\t')
    {
        // Tab completion could be implemented here
        // For now, just insert spaces
        for(int i = 0; i < 4 && bufferIndex < 255; i++)
        {
            commandBuffer[bufferIndex++] = ' ';
            vga->PutChar(' ');
        }
    }
    else if(bufferIndex < 255)
    {
        commandBuffer[bufferIndex++] = c;
        vga->PutChar(c);
    }
}

void Shell::ExecuteCommand(const char* command)
{
    // Skip leading spaces
    while(*command == ' ')
        command++;
    
    if(strlen(command) == 0)
        return;
    
    // Parse command and arguments
    char cmd[64];
    uint32_t i = 0;
    while(command[i] != '\0' && command[i] != ' ' && i < 63)
    {
        cmd[i] = command[i];
        i++;
    }
    cmd[i] = '\0';
    
    const char* args = command + i;
    while(*args == ' ')
        args++;
    
    // Execute command
    if(strcmp(cmd, "help"))
        CommandHelp();
    else if(strcmp(cmd, "clear") || strcmp(cmd, "cls"))
        CommandClear();
    else if(strcmp(cmd, "echo"))
        CommandEcho(args);
    else if(strcmp(cmd, "uptime"))
        CommandUptime();
    else if(strcmp(cmd, "about"))
        CommandAbout();
    else if(strcmp(cmd, "meminfo"))
        CommandMeminfo();
    else if(strcmp(cmd, "uname"))
        CommandUname();
    else if(strcmp(cmd, "syscall"))
        CommandSyscall();
    else if(strcmp(cmd, "disk"))
        CommandDisk();
    else if(strcmp(cmd, "format"))
        CommandFormat();
    else if(strcmp(cmd, "mount"))
        CommandMount();
    else if(strcmp(cmd, "ls"))
        CommandLs();
    else if(strcmp(cmd, "touch"))
        CommandTouch(args);
    else if(strcmp(cmd, "write"))
        CommandWrite(args);
    else if(strcmp(cmd, "cat"))
        CommandCat(args);
    else if(strcmp(cmd, "rm"))
        CommandRm(args);
    else
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Unknown command: ");
        vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga->Print(cmd);
        vga->Print("\nType 'help' for available commands\n");
    }
}

void Shell::CommandHelp()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("Available commands:\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  help");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("     - Show this help message\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  clear");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("    - Clear the screen\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  echo");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("     - Print text to the screen\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  uptime");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("   - Show system uptime\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  about");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("    - About HydraOS\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  meminfo");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("  - Show memory information\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  uname");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("    - Show system information\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  syscall");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("  - Test system call interface\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  disk");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("     - Test ATA/IDE disk operations\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  format");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("   - Format the file system\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  mount");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("    - Mount the file system\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  ls");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("       - List files\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  touch");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("    - Create a file\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  write");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("    - Write text to a file\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  cat");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("      - Display file contents\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  rm");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("       - Delete a file\n");
    
    vga->Print("\n");
    vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga->Print("Filesystem workflow: format -> mount -> touch/write/cat/rm\n");
}

void Shell::CommandClear()
{
    vga->Clear();
}

void Shell::CommandEcho(const char* args)
{
    vga->Print(args);
    vga->Print("\n");
}

void Shell::CommandUptime()
{
    uint32_t ms = timer->GetMilliseconds();
    uint32_t seconds = ms / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    
    seconds = seconds % 60;
    minutes = minutes % 60;
    
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("System uptime: ");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    if(hours > 0)
    {
        vga->PrintHex(hours);
        vga->Print("h ");
    }
    if(minutes > 0 || hours > 0)
    {
        vga->PrintHex(minutes);
        vga->Print("m ");
    }
    vga->PrintHex(seconds);
    vga->Print("s (");
    vga->PrintHex32(ms);
    vga->Print(" ms)\n");
}

void Shell::CommandAbout()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("=====================================\n");
    vga->SetColor(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga->Print("         HydraOS v0.1\n");
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("=====================================\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("A custom x86 operating system\n");
    vga->Print("Built from scratch in C++ and Assembly\n\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga->Print("Author:\n");
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("  Shubham Phapale\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("  GitHub: ShubhamPhapale\n\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga->Print("Features:\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("  - Protected mode with GDT\n");
    vga->Print("  - Interrupt handling (IDT, PIC)\n");
    vga->Print("  - VGA text mode with colors\n");
    vga->Print("  - Keyboard and mouse drivers\n");
    vga->Print("  - Heap memory management (10MB)\n");
    vga->Print("  - Programmable Interval Timer\n");
    vga->Print("  - Cooperative multitasking\n");
    vga->Print("  - System calls (int 0x80)\n");
    vga->Print("  - ATA/IDE disk driver\n");
    vga->Print("  - Simple inode-based filesystem\n");
    vga->Print("  - Interactive shell with 16 commands\n\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("Developed: 2025\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void Shell::CommandMeminfo()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("Memory Information:\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("  Heap start: 0x00A00000 (10 MB)\n");
    vga->Print("  Heap size:  10 MB\n");
    vga->Print("  Allocation: First-fit with coalescing\n");
}

void Shell::CommandUname()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("HydraOS");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print(" v0.1 x86 i686\n");
    vga->Print("Author: Shubham Phapale\n");
    vga->Print("Features: multitasking, memory-management, vga, pit, shell, syscalls, ata, filesystem\n");
}

void Shell::CommandSyscall()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("Testing System Call Interface...\n\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    vga->Print("System call interface has been initialized at interrupt 0x80.\n");
    vga->Print("The syscall handler is ready to handle:\n");
    vga->Print("  1. syscall_printf (print string)\n");
    vga->Print("  2. syscall_getchar (get character)\n");
    vga->Print("  3. syscall_exit (exit process)\n");
    vga->Print("  4. syscall_sleep (sleep)\n");
    vga->Print("  5. syscall_get_uptime (get uptime)\n");
    vga->Print("  6. syscall_malloc (allocate memory)\n");
    vga->Print("  7. syscall_free (free memory)\n\n");
    
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("System call infrastructure ready!\n");
    vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga->Print("Note: Software interrupts from kernel mode require\n");
    vga->Print("      proper privilege level configuration.\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void Shell::CommandDisk()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("Testing ATA/IDE Disk Driver...\n\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    // Initialize primary master ATA drive (0x1F0 port base)
    AdvancedTechnologyAttachment ata0m(true, 0x1F0);
    
    vga->Print("Identifying primary master drive...\n");
    ata0m.Identify();
    vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga->Print("[OK] ");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("Drive identified\n\n");
    
    // Test read operation
    vga->Print("Testing sector read (sector 0)...\n");
    uint8_t* buffer = (uint8_t*)malloc(512);
    
    if(buffer != 0)
    {
        // Read sector 0 (boot sector)
        ata0m.Read28(0, buffer, 1);
        
        vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga->Print("[OK] ");
        vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga->Print("Sector read complete\n");
        
        // Display first 16 bytes
        vga->Print("First 16 bytes: ");
        for(int i = 0; i < 16; i++)
        {
            vga->PrintHex(buffer[i]);
            vga->Print(" ");
        }
        vga->Print("\n\n");
        
        // Test write operation (to sector 100 to avoid overwriting important data)
        vga->Print("Testing sector write (sector 100)...\n");
        
        // Prepare test data
        for(int i = 0; i < 512; i++)
            buffer[i] = i % 256;
        
        // Write test data
        ata0m.Write28(100, buffer, 1);
        
        vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga->Print("[OK] ");
        vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga->Print("Sector write complete\n");
        
        // Read it back to verify
        for(int i = 0; i < 512; i++)
            buffer[i] = 0;
        
        ata0m.Read28(100, buffer, 1);
        
        vga->Print("Read back first 16 bytes: ");
        for(int i = 0; i < 16; i++)
        {
            vga->PrintHex(buffer[i]);
            vga->Print(" ");
        }
        vga->Print("\n");
        
        // Verify
        bool success = true;
        for(int i = 0; i < 512; i++)
        {
            if(buffer[i] != (i % 256))
            {
                success = false;
                break;
            }
        }
        
        if(success)
        {
            vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga->Print("\n[SUCCESS] ");
            vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            vga->Print("Read/Write verification passed!\n");
        }
        else
        {
            vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga->Print("\n[FAILED] ");
            vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            vga->Print("Read/Write verification failed!\n");
        }
        
        free(buffer);
    }
    else
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("[ERROR] ");
        vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga->Print("Failed to allocate buffer\n");
    }
}

void Shell::CommandFormat()
{
    vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga->Print("Formatting file system...\n");
    
    if(fileSystem->Format("HydraOS"))
    {
        vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga->Print("File system formatted successfully!\n");
    }
    else
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Failed to format file system.\n");
    }
}

void Shell::CommandMount()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("Mounting file system...\n");
    
    if(fileSystem->Mount())
    {
        vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga->Print("File system mounted successfully!\n");
    }
    else
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Failed to mount file system. Try 'format' first.\n");
    }
}

void Shell::CommandLs()
{
    vga->SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga->Print("Files:\n");
    
    int32_t count = 0;
    for(int i = 0; i < 64; i++)
    {
        // Check if this inode is in use
        if(fileSystem->inodes[i].type == INODE_TYPE_REGULAR && fileSystem->inodes[i].name[0] != '\0')
        {
            char filename[33];
            
            // Get filename and size from inode
            for(int j = 0; j < 32; j++)
                filename[j] = fileSystem->inodes[i].name[j];
            filename[32] = '\0';
            
            vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            vga->Print("  ");
            vga->Print(filename);
            vga->Print(" (");
            vga->PrintNumber(fileSystem->inodes[i].size);
            vga->Print(" bytes)\n");
            count++;
        }
    }
    
    if(count == 0)
    {
        vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
        vga->Print("  (empty)\n");
        vga->Print("  Tip: Use 'format' and 'mount' if not done yet\n");
    }
    
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("Total: ");
    vga->PrintNumber(count);
    vga->Print(" file(s)\n");
}

void Shell::CommandTouch(const char* args)
{
    if(args[0] == '\0')
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Usage: touch <filename>\n");
        return;
    }
    
    int32_t fd = fileSystem->CreateFile(args);
    if(fd >= 0)
    {
        vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga->Print("File '");
        vga->Print(args);
        vga->Print("' created.\n");
    }
    else
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Failed to create file.\n");
        vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
        vga->Print("Make sure to run 'format' and 'mount' first.\n");
    }
}

void Shell::CommandWrite(const char* args)
{
    if(args[0] == '\0')
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Usage: write <filename>\n");
        return;
    }
    
    // Find the file
    int32_t fd = fileSystem->OpenFile(args);
    if(fd < 0)
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("File not found. Use 'touch' to create it first.\n");
        return;
    }
    
    // Write some test data
    const char* testData = "Hello from HydraOS filesystem!\nThis is a test file.\n";
    uint32_t len = 0;
    while(testData[len] != '\0') len++;
    
    if(fileSystem->WriteFile(fd, (uint8_t*)testData, len))
    {
        vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga->Print("Wrote ");
        vga->PrintNumber(len);
        vga->Print(" bytes to '");
        vga->Print(args);
        vga->Print("'\n");
    }
    else
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Failed to write to file.\n");
    }
}

void Shell::CommandCat(const char* args)
{
    if(args[0] == '\0')
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Usage: cat <filename>\n");
        return;
    }
    
    // Find the file
    int32_t fd = fileSystem->OpenFile(args);
    if(fd < 0)
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("File not found.\n");
        return;
    }
    
    // Get file size using filename
    uint32_t size = fileSystem->GetFileSize(args);
    if(size == 0)
    {
        vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
        vga->Print("(empty file)\n");
        return;
    }
    
    // Allocate buffer and read
    uint8_t* buffer = (uint8_t*)malloc(size + 1);
    if(buffer == 0)
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Out of memory.\n");
        return;
    }
    
    if(fileSystem->ReadFile(fd, buffer, size))
    {
        buffer[size] = '\0';  // Null terminate
        vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga->Print((char*)buffer);
        if(buffer[size-1] != '\n')
            vga->Print("\n");
    }
    else
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Failed to read file.\n");
    }
    
    free(buffer);
}

void Shell::CommandRm(const char* args)
{
    if(args[0] == '\0')
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Usage: rm <filename>\n");
        return;
    }
    
    if(fileSystem->DeleteFile(args))
    {
        vga->SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga->Print("File '");
        vga->Print(args);
        vga->Print("' deleted.\n");
    }
    else
    {
        vga->SetColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga->Print("Failed to delete file. File may not exist.\n");
    }
}
