#ifndef __HYDRAOS__DRIVERS__KEYBOARD_H
#define __HYDRAOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>

namespace hydraos
{
    namespace drivers
    {
        class KeyboardEventHandler
        {
            public:
                KeyboardEventHandler();

                virtual void OnKeyDown(char);
                virtual void OnKeyUp(char);
        };

        class KeyboardDriver : public hydraos::hardwarecommunication::InterruptHandler, public Driver
        {
                hydraos::hardwarecommunication::Port8Bit dataport;
                hydraos::hardwarecommunication::Port8Bit commandport;

                KeyboardEventHandler* handler;

            public:
                KeyboardDriver(hydraos::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler* handler);
                ~KeyboardDriver();
                virtual hydraos::common::uint32_t HandleInterrupt(hydraos::common::uint32_t esp);
                virtual void Activate();
        };
    }
}

#endif