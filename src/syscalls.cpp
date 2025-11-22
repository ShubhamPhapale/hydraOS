#include <syscalls.h>
#include <multitasking.h>
#include <memory.h>

using namespace hydraos;
using namespace hydraos::common;
using namespace hydraos::hardwarecommunication;
using namespace hydraos::drivers;

SystemCallHandler::SystemCallHandler(InterruptManager* interruptManager, uint8_t interruptNumber, VGATextMode* vgaDriver)
    : InterruptHandler(interruptNumber, interruptManager)
{
    this->vga = vgaDriver;
}

SystemCallHandler::~SystemCallHandler()
{
}

uint32_t SystemCallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState* cpu = (CPUState*)esp;
    
    // System call number is in EAX
    uint32_t syscallNumber = cpu->eax;
    
    // Arguments in EBX, ECX, EDX, ESI, EDI
    uint32_t arg1 = cpu->ebx;
    uint32_t arg2 = cpu->ecx;
    uint32_t arg3 = cpu->edx;
    
    // Return value goes in EAX
    switch(syscallNumber)
    {
        case SYSCALL_PRINTF:
        {
            const char* str = (const char*)arg1;
            if(vga != 0)
            {
                vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
                vga->Print("[SYSCALL START] arg1=0x");
                vga->PrintHex32(arg1);
                vga->Print("\n");
                
                if(str != 0)
                {
                    vga->Print("String ptr: 0x");
                    vga->PrintHex32((uint32_t)str);
                    vga->Print(" Content: ");
                    vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    vga->Print(str);
                }
                else
                {
                    vga->Print("(null string)\n");
                }
                vga->SetColor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
                vga->Print("[SYSCALL END]\n");
                vga->SetColor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
            cpu->eax = 0; // Success
            break;
        }
        
        case SYSCALL_GETCHAR:
        {
            // This would need keyboard buffer implementation
            cpu->eax = 0; // Not implemented yet
            break;
        }
        
        case SYSCALL_EXIT:
        {
            // Exit system call - for now just return
            // In a real OS, this would terminate the process
            cpu->eax = arg1; // Return the exit code
            break;
        }
        
        case SYSCALL_SLEEP:
        {
            // Sleep for arg1 milliseconds
            // This is a simplified implementation
            for(uint32_t i = 0; i < arg1 * 1000; i++)
                asm volatile("nop");
            cpu->eax = 0;
            break;
        }
        
        case SYSCALL_GET_UPTIME:
        {
            // Would need timer access - return 0 for now
            cpu->eax = 0;
            break;
        }
        
        case SYSCALL_MALLOC:
        {
            // Would call kernel malloc
            void* ptr = malloc(arg1);
            cpu->eax = (uint32_t)ptr;
            break;
        }
        
        case SYSCALL_FREE:
        {
            // Would call kernel free
            free((void*)arg1);
            cpu->eax = 0;
            break;
        }
        
        default:
        {
            // Unknown system call
            cpu->eax = 0xFFFFFFFF; // Error
            break;
        }
    }
    
    return esp;
}

// System call wrappers - these use inline assembly to trigger int 0x80

void hydraos::syscall_printf(const char* str)
{
    asm volatile(
        "int $0x80"
        :
        : "a"(1), "b"(str)
        : "memory"
    );
}

char hydraos::syscall_getchar()
{
    uint32_t result;
    asm volatile(
        "movl $2, %%eax\n"      // System call number (SYSCALL_GETCHAR)
        "int $0x80\n"
        : "=a"(result)
        :
        : 
    );
    return (char)result;
}

void hydraos::syscall_exit(uint32_t code)
{
    asm volatile(
        "int $0x80"
        :
        : "a"(3), "b"(code)
    );
}

void hydraos::syscall_sleep(uint32_t ms)
{
    asm volatile(
        "int $0x80"
        :
        : "a"(4), "b"(ms)
    );
}

uint32_t hydraos::syscall_get_uptime()
{
    uint32_t result;
    asm volatile(
        "int $0x80"
        : "=a"(result)
        : "a"(5)
    );
    return result;
}

void* hydraos::syscall_malloc(uint32_t size)
{
    void* result;
    asm volatile(
        "int $0x80"
        : "=a"(result)
        : "a"(6), "b"(size)
    );
    return result;
}

void hydraos::syscall_free(void* ptr)
{
    asm volatile(
        "int $0x80"
        :
        : "a"(7), "b"(ptr)
    );
}
