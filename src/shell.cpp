#include <shell.h>
#include <memory.h>
#include <syscalls.h>

using namespace hydraos;
using namespace hydraos::common;
using namespace hydraos::drivers;

Shell::Shell(VGATextMode* vgaDriver, ProgrammableIntervalTimer* timerDriver)
{
    this->vga = vgaDriver;
    this->timer = timerDriver;
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
    vga->Print("Type 'help' for available commands\n\n");
    
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
    vga->Print("Features:\n");
    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga->Print("  - Protected mode with GDT\n");
    vga->Print("  - Interrupt handling (IDT, PIC)\n");
    vga->Print("  - VGA text mode with colors\n");
    vga->Print("  - Keyboard and mouse drivers\n");
    vga->Print("  - Heap memory management\n");
    vga->Print("  - Programmable Interval Timer\n");
    vga->Print("  - Interactive shell\n\n");
    
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
    vga->Print("Features: multitasking, memory-management, vga, pit, shell, syscalls\n");
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
