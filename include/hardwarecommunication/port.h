#ifndef __HYDRAOS__HARDWARECOMMUNICATION__PORT_H
#define __HYDRAOS__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>

namespace hydraos {
    namespace hardwarecommunication {
        class Port {
            protected:
                hydraos::common::uint16_t portnumber;
                Port(hydraos::common::uint16_t portnumber);
                ~Port();
        };

        class Port8Bit : public Port {
            public:
                Port8Bit(hydraos::common::uint16_t portnumber);
                ~Port8Bit();

                virtual void Write(hydraos::common::uint8_t data);
                virtual hydraos::common::uint8_t Read();
        };

        class Port8BitSlow : public Port8Bit {
            public:
                Port8BitSlow(hydraos::common::uint16_t portnumber);
                ~Port8BitSlow();

                virtual void Write(hydraos::common::uint8_t data);
        };

        class Port16Bit : public Port {
            public:
                Port16Bit(hydraos::common::uint16_t portnumber);
                ~Port16Bit();

                virtual void Write(hydraos::common::uint16_t data);
                virtual hydraos::common::uint16_t Read();
        };

        class Port32Bit : public Port {
            public:
                Port32Bit(hydraos::common::uint16_t portnumber);
                ~Port32Bit();

                virtual void Write(hydraos::common::uint32_t data);
                virtual hydraos::common::uint32_t Read();
        };
    }
}

#endif