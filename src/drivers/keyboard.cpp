#include <drivers/keyboard.h>

using namespace hydraos::common;
using namespace hydraos::hardwarecommunication;
using namespace hydraos::drivers;

KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::OnKeyDown(char)
{
}

void KeyboardEventHandler::OnKeyUp(char)
{
}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler)
    : InterruptHandler(manager->HardwareInterruptOffset() + 0x01, manager),
    dataport(0x60),
    commandport(0x64)
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(const char*);
void printfHex(uint8_t);

void KeyboardDriver::Activate()
{
    // Clear the output buffer
    while(commandport.Read() & 0x1)
        dataport.Read();
    
    // Activate keyboard interrupts
    commandport.Write(0xae); // Enable first PS/2 port
    commandport.Write(0x20); // Read Controller Command Byte
    
    // Wait for response
    while(!(commandport.Read() & 0x1))
        ;
    
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // Write Controller Command Byte
    dataport.Write(status);
    
    // Enable keyboard
    dataport.Write(0xf4);
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();

    if(handler == 0)
        return esp;

    if(key < 0x80)
    {
        switch(key)
        {
            // Special keys
            case 0x01: handler->OnKeyDown('\x1B'); break; // ESC
            case 0x0E: handler->OnKeyDown('\b'); break;   // Backspace
            case 0x0F: handler->OnKeyDown('\t'); break;   // Tab
            case 0x1C: handler->OnKeyDown('\n'); break;   // Enter
            case 0x39: handler->OnKeyDown(' '); break;    // Space
            
            // Number row
            case 0x02: handler->OnKeyDown('1'); break;
            case 0x03: handler->OnKeyDown('2'); break;
            case 0x04: handler->OnKeyDown('3'); break;
            case 0x05: handler->OnKeyDown('4'); break;
            case 0x06: handler->OnKeyDown('5'); break;
            case 0x07: handler->OnKeyDown('6'); break;
            case 0x08: handler->OnKeyDown('7'); break;
            case 0x09: handler->OnKeyDown('8'); break;
            case 0x0A: handler->OnKeyDown('9'); break;
            case 0x0B: handler->OnKeyDown('0'); break;
            case 0x0C: handler->OnKeyDown('-'); break;
            case 0x0D: handler->OnKeyDown('='); break;

            // Top row (QWERTY)
            case 0x10: handler->OnKeyDown('q'); break;
            case 0x11: handler->OnKeyDown('w'); break;
            case 0x12: handler->OnKeyDown('e'); break;
            case 0x13: handler->OnKeyDown('r'); break;
            case 0x14: handler->OnKeyDown('t'); break;
            case 0x15: handler->OnKeyDown('y'); break;
            case 0x16: handler->OnKeyDown('u'); break;
            case 0x17: handler->OnKeyDown('i'); break;
            case 0x18: handler->OnKeyDown('o'); break;
            case 0x19: handler->OnKeyDown('p'); break;
            case 0x1A: handler->OnKeyDown('['); break;
            case 0x1B: handler->OnKeyDown(']'); break;

            // Middle row (ASDF)
            case 0x1E: handler->OnKeyDown('a'); break;
            case 0x1F: handler->OnKeyDown('s'); break;
            case 0x20: handler->OnKeyDown('d'); break;
            case 0x21: handler->OnKeyDown('f'); break;
            case 0x22: handler->OnKeyDown('g'); break;
            case 0x23: handler->OnKeyDown('h'); break;
            case 0x24: handler->OnKeyDown('j'); break;
            case 0x25: handler->OnKeyDown('k'); break;
            case 0x26: handler->OnKeyDown('l'); break;
            case 0x27: handler->OnKeyDown(';'); break;
            case 0x28: handler->OnKeyDown('\''); break;
            case 0x29: handler->OnKeyDown('`'); break;

            // Bottom row (ZXCV)
            case 0x2B: handler->OnKeyDown('\\'); break;
            case 0x2C: handler->OnKeyDown('z'); break;
            case 0x2D: handler->OnKeyDown('x'); break;
            case 0x2E: handler->OnKeyDown('c'); break;
            case 0x2F: handler->OnKeyDown('v'); break;
            case 0x30: handler->OnKeyDown('b'); break;
            case 0x31: handler->OnKeyDown('n'); break;
            case 0x32: handler->OnKeyDown('m'); break;
            case 0x33: handler->OnKeyDown(','); break;
            case 0x34: handler->OnKeyDown('.'); break;
            case 0x35: handler->OnKeyDown('/'); break;

            // Shift, Ctrl, Alt (just ignore for now)
            case 0x2A: break; // Left Shift
            case 0x36: break; // Right Shift
            case 0x1D: break; // Left Ctrl
            case 0x38: break; // Left Alt
            case 0x3A: break; // Caps Lock

            default:
            {
                // Silently ignore unmapped keys
                break;
            }
        }
    }
    return esp;
}