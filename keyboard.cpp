#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
    : InterruptHandler(0x21, manager),
    dataport(0x60),
    commandport(0x64)
{
    while (commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xAE); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    dataport.Write(0xF4);
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(const char* str);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();

    if(key < 0x80)
    {
        switch(key)
        {
            case 0xFA: break;
            
            case 0x1E: printf("a"); break; 

            case 0x45:
            case 0xC5: break;
            default:
            {
                char* foo = "KEYBOARD 0x00";
                char* hex = "0123456789ABCDEF";
                foo[11] = hex[(key >> 4) & 0x0F];
                foo[12] = hex[key & 0x0F];
                printf(foo);
                break;
            }
        }
    }
    return esp;
}