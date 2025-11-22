#ifndef __HYDRAOS__MULTITASKING_H
#define __HYDRAOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace hydraos
{
    struct CPUState
    {
        hydraos::common::uint32_t eax;
        hydraos::common::uint32_t ebx;
        hydraos::common::uint32_t ecx;
        hydraos::common::uint32_t edx;
        
        hydraos::common::uint32_t esi;
        hydraos::common::uint32_t edi;
        hydraos::common::uint32_t ebp;
        
        hydraos::common::uint32_t error;
        
        hydraos::common::uint32_t eip;
        hydraos::common::uint32_t cs;
        hydraos::common::uint32_t eflags;
        hydraos::common::uint32_t esp;
        hydraos::common::uint32_t ss;
    } __attribute__((packed));
    
    class Task
    {
        friend class TaskManager;
        
        private:
            hydraos::common::uint8_t stack[4096]; // 4KB stack per task
            CPUState* cpustate;
            
        public:
            Task(GlobalDescriptorTable* gdt, void entrypoint());
            ~Task();
    };
    
    class TaskManager
    {
        private:
            Task* tasks[256];
            hydraos::common::int32_t numTasks;
            hydraos::common::int32_t currentTask;
            
        public:
            TaskManager();
            ~TaskManager();
            
            bool AddTask(Task* task);
            CPUState* Schedule(CPUState* cpustate);
    };
}

#endif
