#include <multitasking.h>

using namespace hydraos;
using namespace hydraos::common;

Task::Task(GlobalDescriptorTable* gdt, void entrypoint())
{
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    
    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = 0;
    
    cpustate->esi = 0;
    cpustate->edi = 0;
    cpustate->ebp = 0;
    
    cpustate->eip = (uint32_t)entrypoint;
    cpustate->cs = gdt->CodeSegmentSelector();
    cpustate->eflags = 0x202; // Interrupt enable flag
}

Task::~Task()
{
}

TaskManager::TaskManager()
{
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{
}

bool TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    
    tasks[numTasks++] = task;
    return true;
}

CPUState* TaskManager::Schedule(CPUState* cpustate)
{
    if(numTasks <= 0)
        return cpustate;
    
    // Save current task state
    if(currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;
    
    // Round-robin scheduling
    currentTask++;
    if(currentTask >= numTasks)
        currentTask = 0;
    
    // Return next task's state
    return tasks[currentTask]->cpustate;
}
