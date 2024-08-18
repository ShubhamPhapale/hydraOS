#ifndef __HYDRAOS__DRIVERS__MOUSE_H
#define __HYDRAOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>

namespace hydraos
{
    namespace drivers
    {
        class MouseEventHandler
        {
            public:
                MouseEventHandler();

                virtual void OnActivate();
                virtual void OnMouseDown(hydraos::common::uint8_t button);
                virtual void OnMouseUp(hydraos::common::uint8_t button);
                virtual void OnMouseMove(int x, int y);
        };

        class MouseDriver : public hydraos::hardwarecommunication::InterruptHandler, public Driver
        {
                hydraos::hardwarecommunication::Port8Bit dataport;
                hydraos::hardwarecommunication::Port8Bit commandport;

                hydraos::common::uint8_t buffer[3];
                hydraos::common::uint8_t offset;
                hydraos::common::uint8_t buttons;

                MouseEventHandler* handler;
            public:
                MouseDriver(hydraos::hardwarecommunication::InterruptManager* manager, MouseEventHandler* handler);
                ~MouseDriver();
                virtual hydraos::common::uint32_t HandleInterrupt(hydraos::common::uint32_t esp);
                virtual void Activate();
        };
    }
}

#endif