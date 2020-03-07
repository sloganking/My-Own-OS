#ifndef __MYOS__HARDWARECOMMUNICTION__INTERRUPTS_H
#define __MYOS__HARDWARECOMMUNICTION__INTERRUPTS_H

#include <common/types.h>
#include <multitasking.h>
#include <hardwarecommunication/port.h>
#include <gdt.h>

namespace myos{
    namespace hardwarecommunication{
        class InterruptManager;

        class InterruptHandler{
            protected:
                myos::common::uint8_t interruptNumber;
                InterruptManager* interruptManager;

                InterruptHandler(myos::common::uint8_t interruptNumber, InterruptManager* interruptManager);
                ~InterruptHandler();
            public:
                virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
        };

        class InterruptManager{

            friend class InterruptHandler;

            // interrupt descriptor table
            protected:

                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* handlers[256];
                TaskManager *taskManager;

                struct GateDescriptor{
                    myos::common::uint16_t handlerAddressLowBits;
                    //offset of GDT
                    myos::common::uint16_t gdt_codeSegmentSelector;
                    myos::common::uint8_t reserved;
                    myos::common::uint8_t access;
                    myos::common::uint16_t handlerAddressHighBits;

                } __attribute__((packed));

                //for this structure,
                //create array with 256 entries

                static GateDescriptor interruptDescriptorTable[256];

                struct InterruptDescriptorTablePointer{
                    myos::common::uint16_t size;
                    myos::common::uint32_t base;  //address of table
                } __attribute__((packed));

                static void SetInterruptDescriptorTableEntry(
                    myos::common::uint8_t interruptNumber,
                    myos::common::uint16_t codeSegmentSelectorOffset,
                    void (*handler)(),      //pointer to handler
                    myos::common::uint8_t DescriptorPrivilegeLevel,
                    myos::common::uint8_t DescriptorType
                );


                Port8BitSlow picMasterCommand;
                Port8BitSlow picMasterData;
                Port8BitSlow picSlaveCommand;
                Port8BitSlow picSlaveData;

            public:

                //constructor
                InterruptManager(GlobalDescriptorTable* gdt, TaskManager* taskManager);

                //descructor
                ~InterruptManager();

                void Activate();
                void Deactivate();

                static myos::common::uint32_t HandleInterrupt(myos::common::uint8_t interruptNumber, myos::common::uint32_t esp);
                myos::common::uint32_t DoHandleInterrupt(myos::common::uint8_t interruptNumber, myos::common::uint32_t esp);

                static void IgnoreInterruptRequest();
                static void HandleInterruptRequest0x00();   //timer
                static void HandleInterruptRequest0x01();   //keyboard
                static void HandleInterruptRequest0x0C();   //mouse
        };
    }
}
#endif