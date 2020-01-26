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

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){

    //fetch the pressed key
    uint8_t key = dataport.Read();

    //anything above 0x80 signifies a key lift. we will ignore those for now
    if(key < 0x80){
        switch(key){
            case 0x02: printf("1"); break;
            case 0x03: printf("2"); break;
            case 0x04: printf("3"); break;
            case 0x05: printf("4"); break;
            case 0x06: printf("5"); break;
            case 0x07: printf("6"); break;
            case 0x08: printf("7"); break;
            case 0x09: printf("8"); break;
            case 0x0A: printf("9"); break;
            case 0x0B: printf("0"); break;

            case 0x10: printf("q"); break;
            case 0x11: printf("w"); break;
            case 0x12: printf("e"); break;
            case 0x13: printf("r"); break;
            case 0x14: printf("t"); break;
            case 0x15: printf("y"); break;
            case 0x16: printf("u"); break;
            case 0x17: printf("i"); break;
            case 0x18: printf("o"); break;
            case 0x19: printf("p"); break;

            case 0x1E: printf("a"); break;
            case 0x1F: printf("s"); break;
            case 0x20: printf("d"); break;
            case 0x21: printf("f"); break;
            case 0x22: printf("g"); break;
            case 0x23: printf("h"); break;
            case 0x24: printf("j"); break;
            case 0x25: printf("k"); break;
            case 0x26: printf("l"); break;

            case 0x2C: printf("z"); break;
            case 0x2D: printf("x"); break;
            case 0x2E: printf("c"); break;
            case 0x2F: printf("v"); break;
            case 0x30: printf("b"); break;
            case 0x31: printf("n"); break;
            case 0x32: printf("m"); break;
            case 0x33: printf(","); break;
            case 0x34: printf("."); break;
            case 0x35: printf("-"); break;

            case 0x1C: printf("\n"); break;
            case 0x39: printf(" "); break;

            default:
                //print number of interrupt we didn't handle
                char* foo = "KEYBOARD 0x00 ";
                char* hex = "0123456789ABCDF";
                foo[11] = hex[(key >> 4) & 0x0F];
                foo[12] = hex[key & 0x0F];
                printf(foo);
        }
    }

    return esp;
}