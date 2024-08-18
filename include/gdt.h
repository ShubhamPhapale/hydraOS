#ifndef __HYDRAOS__GDT_H
#define __HYDRAOS__GDT_H

#include <common/types.h>

namespace hydraos
{
    class GlobalDescriptorTable 
    {
        public:
            class SegmentDescriptor 
            {
                private:
                    hydraos::common::uint16_t limit_lo;
                    hydraos::common::uint16_t base_lo;
                    hydraos::common::uint8_t base_hi;
                    hydraos::common::uint8_t type;
                    hydraos::common::uint8_t limit_hi;
                    hydraos::common::uint8_t base_vhi;
                public:
                    SegmentDescriptor(hydraos::common::uint32_t base, hydraos::common::uint32_t limit, hydraos::common::uint8_t type);
                    hydraos::common::uint32_t Base();
                    hydraos::common::uint32_t Limit();
            } __attribute__((packed));

            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;

        public:
            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            hydraos::common::uint16_t CodeSegmentSelector();
            hydraos::common::uint16_t DataSegmentSelector();
    };
}

#endif