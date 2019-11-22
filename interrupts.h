#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager{

    // interrupt descriptor table
    protected:
        struct GateDescriptor{
            uint16_t handlerAddressLowBits;
            //offset of GDT
            uint16_t gdt_codeSegmentSelector;
            uint8_t reserved;
            uint8_t access;
            uint16_t handlerAddressHighBits

        }   __attribute_((packed));

        //for this structure,
        //create array with 256 entries

        static GateDescriptor interruptDescriptorTable[256];

        struct interruptDescriptorTablePointer{
            uint16_t size;
            uint32_t base;  //address of table
        } __attribute_((packed));

        static void SetInterruptDescriptorTableEntry(
            uint8_t interruptNumber,
            uint16_t codeSegmentSelectorOffset,
            void (*handler)(),      //pointer to handler
            uint8_t DescriptorPrivilegeLevel,
            uint8_t DescriptorType
        );

    public:

        //constructor
        InterruptManager(GlobalDDescriptorTable* gdt);

        //descructor
        ~InterruptManager();

        static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);

        static void IgnoreInterruptRequest();
        static void handleInterruptRequest0x00();   //timer
        static void handleInterruptRequest0x01();   //keyboard
};

#endif