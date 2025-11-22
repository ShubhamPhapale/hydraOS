#ifndef __HYDRAOS__SHELL_H
#define __HYDRAOS__SHELL_H

#include <common/types.h>
#include <drivers/vga.h>
#include <drivers/pit.h>

namespace hydraos
{
    class Shell
    {
        private:
            drivers::VGATextMode* vga;
            drivers::ProgrammableIntervalTimer* timer;
            char commandBuffer[256];
            hydraos::common::uint8_t bufferIndex;
            
            // String utility functions
            hydraos::common::uint32_t strlen(const char* str);
            bool strcmp(const char* str1, const char* str2);
            bool strncmp(const char* str1, const char* str2, hydraos::common::uint32_t n);
            void strcpy(char* dest, const char* src);
            
            // Command handlers
            void ExecuteCommand(const char* command);
            void CommandHelp();
            void CommandClear();
            void CommandEcho(const char* args);
            void CommandUptime();
            void CommandAbout();
            void CommandMeminfo();
            void CommandUname();
            void CommandSyscall();
            void CommandDisk();
            
            void PrintPrompt();
            
        public:
            Shell(drivers::VGATextMode* vgaDriver, drivers::ProgrammableIntervalTimer* timerDriver);
            ~Shell();
            
            void HandleKeyPress(char c);
            void Start();
    };
}

#endif
