#include <drivers/pit.h>

using namespace hydraos::common;
using namespace hydraos::drivers;
using namespace hydraos::hardwarecommunication;

ProgrammableIntervalTimer::ProgrammableIntervalTimer(InterruptManager* manager)
    : InterruptHandler(manager->HardwareInterruptOffset() + 0x00, manager),
      dataPort0(0x40),
      commandPort(0x43)
{
    ticks = 0;
    SetFrequency(100); // 100 Hz default (10ms per tick)
}

ProgrammableIntervalTimer::~ProgrammableIntervalTimer()
{
}

void ProgrammableIntervalTimer::SetFrequency(uint32_t freq)
{
    if(freq == 0)
        return;
        
    frequency = freq;
    
    // PIT input frequency is 1193180 Hz
    uint32_t divisor = 1193180 / freq;
    
    // Command byte: channel 0, lobyte/hibyte, rate generator
    commandPort.Write(0x36);
    
    // Send frequency divisor
    dataPort0.Write((uint8_t)(divisor & 0xFF));
    dataPort0.Write((uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t ProgrammableIntervalTimer::GetFrequency()
{
    return frequency;
}

uint32_t ProgrammableIntervalTimer::GetTicks()
{
    return ticks;
}

uint32_t ProgrammableIntervalTimer::GetMilliseconds()
{
    return (ticks * 1000) / frequency;
}

void ProgrammableIntervalTimer::Sleep(uint32_t milliseconds)
{
    uint32_t startTicks = ticks;
    uint32_t targetTicks = (milliseconds * frequency) / 1000;
    
    while((ticks - startTicks) < targetTicks)
    {
        // Busy wait
        asm volatile("nop");
    }
}

uint32_t ProgrammableIntervalTimer::HandleInterrupt(uint32_t esp)
{
    ticks++;
    return esp;
}
