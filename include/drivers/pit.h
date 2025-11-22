#ifndef __HYDRAOS__DRIVERS__PIT_H
#define __HYDRAOS__DRIVERS__PIT_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace hydraos
{
    namespace drivers
    {
        class ProgrammableIntervalTimer : public hydraos::hardwarecommunication::InterruptHandler
        {
            protected:
                hydraos::common::uint32_t frequency;
                hydraos::common::uint32_t ticks;
                
                hydraos::hardwarecommunication::Port8Bit dataPort0;
                hydraos::hardwarecommunication::Port8Bit commandPort;

            public:
                ProgrammableIntervalTimer(hydraos::hardwarecommunication::InterruptManager* manager);
                ~ProgrammableIntervalTimer();

                virtual hydraos::common::uint32_t HandleInterrupt(hydraos::common::uint32_t esp);
                
                void SetFrequency(hydraos::common::uint32_t freq);
                hydraos::common::uint32_t GetFrequency();
                hydraos::common::uint32_t GetTicks();
                hydraos::common::uint32_t GetMilliseconds();
                
                void Sleep(hydraos::common::uint32_t milliseconds);
        };
    }
}

#endif
