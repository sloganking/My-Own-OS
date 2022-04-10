#include <hardwarecommunication/interrupts.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;


void printf(char* str);
void printfHex(uint8_t);

//constructor
InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager){
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}
//destructor
InterruptHandler::~InterruptHandler(){
    if(interruptManager->handlers[interruptNumber] == this){
        interruptManager->handlers[interruptNumber] = 0;
    }
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp){
    return esp;
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::ActiveInterruptManager = 0;

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),      //pointer to handler
    uint8_t DescriptorPrivilegeLevel,
    uint8_t DescriptorType
)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel & 3) << 5);
    interruptDescriptorTable[interruptNumber].reserved = 0;
}


//constructor
InterruptManager::InterruptManager(GlobalDescriptorTable* gdt, TaskManager* taskManager)

//instansiate ports, numbers are engrained in hardware
: picMasterCommand(0x20),
  picMasterData(0x21),
  picSlaveCommand(0xA0),
  picSlaveData(0xA1)

{
    this->taskManager = taskManager;
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for(uint16_t i = 0; i < 256; i++){
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

    //clock
    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);

    //keyboard
    SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

    //mouse
    SetInterruptDescriptorTableEntry(0x2C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    //have PICs add numbers to interupts so we can differentiate between them and exceptions
    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);



    //tell Master PIC it is master, and Slave PIC it is the slave
    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

    //tell the processor to use the created IDT
    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
}

//descructor
InterruptManager::~InterruptManager(){

}

void InterruptManager::Activate(){
    if(ActiveInterruptManager != 0){
        ActiveInterruptManager->Deactivate();
    }
    ActiveInterruptManager = this;
    asm("sti");
}

void InterruptManager::Deactivate(){
    if(ActiveInterruptManager == this){
        ActiveInterruptManager = 0;
        asm("cli");
    }
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interruptNumber, uint32_t esp){

    if(ActiveInterruptManager != 0)
        return ActiveInterruptManager->DoHandleInterrupt(interruptNumber, esp);
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp){

    //if we have a handler for the interrupt number
    if(handlers[interruptNumber] != 0){
        //call it's handle interrupt method
        esp = handlers[interruptNumber]->HandleInterrupt(esp);
    }
    //if not an interrupt from the clock
    else if(interruptNumber != 0x20){

        //print number of interrupt we didn't handle
        printf("UNHANDLED INTERRUPT 0x");
        printfHex(interruptNumber);

    }

    //if clock interrupt
    if(interruptNumber == 0x20){
        //move the stack pointer so that when we ret, a new task will be jumped into
        esp = (uint32_t)taskManager->Schedule((CPUState*)esp);
    }

    //if it's a hardware interrupt
    if(0x20 <= interruptNumber && interruptNumber < 0x30){
        //answer interrupt
        picMasterCommand.Write(0x20);

        // if interupt came from slave, respond to slave
        //slave is 0x28 - 0x30
        if(0x28 <= interruptNumber)
            picSlaveCommand.Write(0x20);
    }

    return esp;
}