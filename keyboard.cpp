#include "keyboard.h"

//forward declaration of printf
//so that we can use printf
void printf(char*);

//constructor
KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
dataport(0x60),
commandport(0x64)
{
    //send multiple key presses while key is held down
    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xAE); // activate interrupts
    commandport.Write(0x20); // get current state

    //set right most bit to one because this will be the new state
    //and clear the 5th bit
    uint8_t status = (dataport.Read() | 1) & ~0x10;

    commandport.Write(0x60); // set current state
    dataport.Write(status);

    dataport.Write(0xF4); // activate keyboard
}

//destructor
KeyboardDriver::~KeyboardDriver(){

}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp){

    //fetch the pressed key
    uint8_t key = dataport.Read();

    //print number of interrupt we didn't handle
    char* foo = "KEYBOARD 0x00";
    char* hex = "0123456789ABCDF";
    foo[11] = hex[(key >> 4) & 0x0F];
    foo[12] = hex[key & 0x0F];
    printf(foo);

    return esp;
}