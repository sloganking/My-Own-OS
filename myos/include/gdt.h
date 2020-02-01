#ifndef __MYOS__GDT_H
#define __MYOS__GDT_H

#include <common/types.h>

namespace myos{
     class GlobalDescriptorTable{
        public:
            class SegmentDescriptor{

                private:
                    myos::common::uint16_t limit_lo;
                    myos::common::uint16_t base_lo;
                    myos::common::uint8_t base_hi;
                    myos::common::uint8_t type;
                    myos::common::uint8_t flags_limit_hi;
                    myos::common::uint8_t base_vhi;
                public:
                    // get regular values and move them around as we want to
                    // stores values how they need to be stored for backwards compatability
                    SegmentDescriptor(myos::common::uint32_t base, myos::common::uint32_t limit, myos::common::uint8_t type);

                    // turns funkilly stored values back into usable values
                    myos::common::uint32_t Base();
                    myos::common::uint32_t Limit();

            } __attribute__((packed)) ;     // prevents compiler from moving objects around

            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;

        public:
            GlobalDescriptorTable();    //contructor
            ~GlobalDescriptorTable();   // desctructor

            myos::common::uint16_t CodeSegmentSelector(); // give offset of codeSegmentDescriptor
            myos::common::uint16_t DataSegmentSelector(); // give offset of dataSegmentDescriptor
    };
}

#endif