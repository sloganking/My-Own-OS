#ifndef __GDT_H
#define __GDT_H

#include "types.h"

    class GlobalDescriptorTable{
        public:
            class SegmentDescriptor{

                private:
                    uint16_t limit_lo;
                    uint16_t base_lo;
                    uint8_t base_hi;
                    uint8_t type;
                    uint8_t flags_limit_hi;
                    uint8_t base_vhi;
                public:
                    // get regular values and move them around as we want to
                    // stores values how they need to be stored for backwards compatability
                    SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);

                    // turns funkilly stored values back into usable values
                    uint32_t Base();
                    uint32_t Limit();

            } __attribute__((packed)) ;     // prevents compiler from moving objects around

            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;

        public:
            GlobalDescriptorTable();    //contructor
            ~GlobalDescriptorTable();   // desctructor

            uint16_t CodeSegmentSelector(); // give offset of codeSegmentDescriptor
            uint16_t DataSegmentSelector(); // give offset of dataSegmentDescriptor
    };

#endif