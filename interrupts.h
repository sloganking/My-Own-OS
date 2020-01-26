#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager;

class InterruptHandler{
protected:
    uint8_t interruptNumber;
    InterruptManager* interruptManager;

    InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
    ~InterruptHandler();
public:
    uint32_t handleInterrupt(uint32_t esp);
};

class InterruptManager{

    friend class InterruptHandler;

    // interrupt descriptor table
    protected:

        static InterruptManager* ActiveInterruptManager;
        InterruptHandler* handlers[256];

        struct GateDescriptor{
            uint16_t handlerAddressLowBits;
            //offset of GDT
            uint16_t gdt_codeSegmentSelector;
            uint8_t reserved;
            uint8_t access;
            uint16_t handlerAddressHighBits;

        } __attribute__((packed));

        //for this structure,
        //create array with 256 entries

        static GateDescriptor interruptDescriptorTable[256];

        struct InterruptDescriptorTablePointer{
            uint16_t size;
            uint32_t base;  //address of table
        } __attribute__((packed));

        static void SetInterruptDescriptorTableEntry(
            uint8_t interruptNumber,
            uint16_t codeSegmentSelectorOffset,
            void (*handler)(),      //pointer to handler
            uint8_t DescriptorPrivilegeLevel,
            uint8_t DescriptorType
        );


        Port8BitSlow picMasterCommand;
        Port8BitSlow picMasterData;
        Port8BitSlow picSlaveCommand;
        Port8BitSlow picSlaveData;

    public:

        //constructor
        InterruptManager(GlobalDescriptorTable* gdt);

        //descructor
        ~InterruptManager();

        void Activate();
        void Deactivate();

        static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);
        uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);

        static void IgnoreInterruptRequest();
        static void HandleInterruptRequest0x00();   //timer
        static void HandleInterruptRequest0x01();   //keyboard
};

#endif