#include <common/types.h>
#include <gdt.h>
#include <memory.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>

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

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
    public:
        void OnKeyDown(char c)
        {
            char* foo = " ";
            foo[0] = c;
            printf(foo);
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

    vga.SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga.Print("\nInitializing Hardware Drivers:\n");

    DriverManager drvManager;
        PrintfKeyboardEventHandler kbhandler;
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
    vga.SetColor(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga.Print("  System Ready! Type to test input.\n");
    vga.SetColor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga.Print("=====================================\n\n");
    
    // Test memory allocation
    vga.SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga.Print("Testing memory allocation...\n");
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    void* ptr1 = malloc(100);
    vga.Print("  malloc(100) = 0x");
    vga.PrintHex32((uint32_t)ptr1);
    vga.Print("\n");
    
    void* ptr2 = malloc(256);
    vga.Print("  malloc(256) = 0x");
    vga.PrintHex32((uint32_t)ptr2);
    vga.Print("\n");
    
    void* ptr3 = malloc(512);
    vga.Print("  malloc(512) = 0x");
    vga.PrintHex32((uint32_t)ptr3);
    vga.Print("\n");
    
    vga.Print("  Freeing first allocation...\n");
    free(ptr1);
    
    vga.Print("  malloc(50) = 0x");
    void* ptr4 = malloc(50);
    vga.PrintHex32((uint32_t)ptr4);
    vga.Print(" (reused freed space)\n");
    
    vga.SetColor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga.Print("  Memory allocation test passed!\n\n");
    
    vga.SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga.Print("> ");
    
    while(1);
}