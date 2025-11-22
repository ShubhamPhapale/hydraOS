/**
 * @file kernel.cpp
 * @brief Main kernel entry point for HydraOS
 * @author Shubham Phapale
 * @date 2025
 */

#include <common/types.h>
#include <gdt.h>
#include <memory.h>
#include <shell.h>
#include <multitasking.h>
#include <syscalls.h>
#include <filesystem.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/pit.h>
#include <drivers/ata.h>

using namespace hydraos;
using namespace hydraos::common;
using namespace hydraos::hardwarecommunication;
using namespace hydraos::drivers;

void printf(const char* str)
{
    static uint16_t*  VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x = 0, y = 0;

    for(int i=0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] =  (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0x0F];
    foo[1] = hex[key & 0x0F];
    printf(foo);
}

// Global shell pointer for keyboard handler
Shell* globalShell = 0;

class ShellKeyboardEventHandler : public KeyboardEventHandler
{
    public:
        void OnKeyDown(char c)
        {
            if(globalShell != 0)
            {
                globalShell->HandleKeyPress(c);
            }
        }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
    public:
        MouseToConsole()
        {
        }

        virtual void OnActivate()
        {
            uint16_t* VideoMemory = (uint16_t*)0xb8000;
            x = 40;
            y = 12;
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                | (VideoMemory[80*y+x] & 0xF000) >> 4
                                | (VideoMemory[80*y+x] & 0x00FF);        
        }

        void OnMouseMove(int xoffset, int yoffset)
        {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xF000) >> 4
                | (VideoMemory[80*y+x] & 0x0F00) << 4
                | (VideoMemory[80*y+x] & 0x00FF);

            x += xoffset;
            if(x < 0) x = 0;
            if(x >= 80) x = 79;

            y += yoffset;
            if(y < 0) y = 0;
            if(y >= 25) y = 24;

            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xF000) >> 4
                | (VideoMemory[80*y+x] & 0x0F00) << 4
                | (VideoMemory[80*y+x] & 0x00FF);
        }
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

// Demo task functions
void taskA()
{
    while(true)
        ;
}

void taskB()
{
    while(true)
        ;
}

extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*magicnumber*/)
{
    // Initialize VGA Text Mode
    VGATextMode vga;
    vga.Clear();
    
    // Print welcome banner with colors
    vga.SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga.Print("=====================================\n");
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("    HydraOS v0.1 - System Boot\n");
    vga.SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga.Print("=====================================\n\n");
    
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("Initializing system components...\n\n");

    GlobalDescriptorTable gdt;
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("Global Descriptor Table (GDT)\n");

    // Initialize memory management with 10MB heap starting at 10MB
    size_t heapSize = 1024 * 1024 * 10; // 10 MB
    MemoryManager memoryManager(1024 * 1024 * 10, heapSize);
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("Memory Manager (Heap: ");
    vga.Print("10 MB)\n");

    InterruptManager interrupts(0x20, &gdt);
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("Interrupt Manager\n");

    // Initialize Programmable Interval Timer (100 Hz)
    ProgrammableIntervalTimer timer(&interrupts);
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("Programmable Interval Timer (100 Hz)\n");

    // Initialize ATA/IDE Driver (Primary Master)
    AdvancedTechnologyAttachment ata0m(true, 0x1F0);
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("ATA/IDE Driver (Primary Master)\n");

    // Initialize File System
    filesystem::FileSystem fileSystem(&ata0m);
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("File System\n");

    // Initialize Shell
    Shell shell(&vga, &timer, &fileSystem);
    globalShell = &shell;
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("Shell\n");

    // Initialize Task Manager
    TaskManager taskManager;
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("Task Manager (2 tasks)\n");

    // Initialize System Call Handler (interrupt 0x80)
    SystemCallHandler syscallHandler(&interrupts, 0x80, &vga);
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("System Call Handler (int 0x80)\n");

    vga.SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga.Print("\nInitializing Hardware Drivers:\n");

    DriverManager drvManager;
        ShellKeyboardEventHandler kbhandler;
        KeyboardDriver keyboard(&interrupts, &kbhandler);
        drvManager.AddDriver(&keyboard);
        
        vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga.Print("  [OK] ");
        vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga.Print("Keyboard Driver\n");

        MouseToConsole mousehandler;
        MouseDriver mouse(&interrupts, &mousehandler);
        drvManager.AddDriver(&mouse);
        
        vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga.Print("  [OK] ");
        vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga.Print("Mouse Driver\n");

    vga.SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga.Print("\nActivating all drivers...\n");
    drvManager.ActivateAll();
    
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("All drivers activated\n");

    vga.SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga.Print("\nEnabling interrupts...\n");
    interrupts.Activate();
    
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("[OK] ");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("Interrupts enabled\n\n");
    
    vga.SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga.Print("=====================================\n");
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("     System Boot Complete!\n");
    vga.SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga.Print("=====================================\n");
    
    // Start the shell
    shell.Start();
    
    while(1);
}