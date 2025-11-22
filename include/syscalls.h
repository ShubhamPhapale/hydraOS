#ifndef __HYDRAOS__SYSCALLS_H
#define __HYDRAOS__SYSCALLS_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/vga.h>

namespace hydraos
{
    // System call numbers
    enum SystemCallNumber
    {
        SYSCALL_PRINTF = 1,
        SYSCALL_GETCHAR = 2,
        SYSCALL_EXIT = 3,
        SYSCALL_SLEEP = 4,
        SYSCALL_GET_UPTIME = 5,
        SYSCALL_MALLOC = 6,
        SYSCALL_FREE = 7
    };
    
    class SystemCallHandler : public hydraos::hardwarecommunication::InterruptHandler
    {
        private:
            drivers::VGATextMode* vga;
            
        public:
            SystemCallHandler(hydraos::hardwarecommunication::InterruptManager* interruptManager,
                            hydraos::common::uint8_t interruptNumber,
                            drivers::VGATextMode* vgaDriver);
            ~SystemCallHandler();
            
            virtual hydraos::common::uint32_t HandleInterrupt(hydraos::common::uint32_t esp);
    };
    
    // System call wrappers for user programs
    void syscall_printf(const char* str);
    char syscall_getchar();
    void syscall_exit(hydraos::common::uint32_t code);
    void syscall_sleep(hydraos::common::uint32_t ms);
    hydraos::common::uint32_t syscall_get_uptime();
    void* syscall_malloc(hydraos::common::uint32_t size);
    void syscall_free(void* ptr);
}

#endif
