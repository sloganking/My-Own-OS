#include "interrupts.h"

void printf(char* str);

InterruptManager::GateDescriptor InterruptManager::SetInterruptDescriptorTable[256];

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),      //pointer to handler
    uint8_t DescriptorPrivilegeLevel,
    uint8_t DescriptorType
);
{
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel & 3) << 5);
    interruptDescriptorTable[interruptNumber].reserved = 0;
}


//constructor
InterruptManager(GlobalDDescriptorTable* gdt){
    uint16_t CodeSegment = gdt->codeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for(uint16_t i = 0; i < 256; i++){
        SetInterruptDescriptorTableEntry(i, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE)
    }

    //clock
    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE)

    //keyboard
    SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE)

    //tell the processor to use the created IDT
    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
}

//descructor
~InterruptManager(){
    
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp){

    printf(" INTERUPT");

    return esp;
}