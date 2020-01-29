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

    static bool Shift = false;
    static bool CapsLock = false;
    static bool UpperCase = false;

    switch(key){
        case 0x02: if(Shift) printf("!"); else printf("1"); break;
        case 0x03: if(Shift) printf("@"); else printf("2"); break;
        case 0x04: if(Shift) printf("#"); else printf("3"); break;
        case 0x05: if(Shift) printf("$"); else printf("4"); break;
        case 0x06: if(Shift) printf("%"); else printf("5"); break;
        case 0x07: if(Shift) printf("^"); else printf("6"); break;
        case 0x08: if(Shift) printf("&"); else printf("7"); break;
        case 0x09: if(Shift) printf("*"); else printf("8"); break;
        case 0x0A: if(Shift) printf("("); else printf("9"); break;
        case 0x0B: if(Shift) printf(")"); else printf("0"); break;

        case 0x10: if(UpperCase) printf("Q"); else printf("q"); break;
        case 0x11: if(UpperCase) printf("W"); else printf("w"); break;
        case 0x12: if(UpperCase) printf("E"); else printf("e"); break;
        case 0x13: if(UpperCase) printf("R"); else printf("r"); break;
        case 0x14: if(UpperCase) printf("T"); else printf("t"); break;
        case 0x15: if(UpperCase) printf("Y"); else printf("y"); break;
        case 0x16: if(UpperCase) printf("U"); else printf("u"); break;
        case 0x17: if(UpperCase) printf("I"); else printf("i"); break;
        case 0x18: if(UpperCase) printf("O"); else printf("o"); break;
        case 0x19: if(UpperCase) printf("P"); else printf("p"); break;

        case 0x1E: if(UpperCase) printf("A"); else printf("a"); break;
        case 0x1F: if(UpperCase) printf("S"); else printf("s"); break;
        case 0x20: if(UpperCase) printf("D"); else printf("d"); break;
        case 0x21: if(UpperCase) printf("F"); else printf("f"); break;
        case 0x22: if(UpperCase) printf("G"); else printf("g"); break;
        case 0x23: if(UpperCase) printf("H"); else printf("h"); break;
        case 0x24: if(UpperCase) printf("J"); else printf("j"); break;
        case 0x25: if(UpperCase) printf("K"); else printf("k"); break;
        case 0x26: if(UpperCase) printf("L"); else printf("l"); break;
        case 0x27: if(Shift) printf(":"); else printf(";"); break;

        case 0x2C: if(UpperCase) printf("Z"); else printf("z"); break;
        case 0x2D: if(UpperCase) printf("X"); else printf("x"); break;
        case 0x2E: if(UpperCase) printf("C"); else printf("c"); break;
        case 0x2F: if(UpperCase) printf("V"); else printf("v"); break;
        case 0x30: if(UpperCase) printf("B"); else printf("b"); break;
        case 0x31: if(UpperCase) printf("N"); else printf("n"); break;
        case 0x32: if(UpperCase) printf("M"); else printf("m"); break;
        case 0x33: if(Shift) printf("<"); else printf(","); break;
        case 0x34: if(Shift) printf(">"); else printf("."); break;
        case 0x35: if(Shift) printf("_"); else printf("-"); break;

        case 0x1A: if(Shift) printf("{"); else printf("["); break;
        case 0x1B: if(Shift) printf("}"); else printf("]"); break;
        case 0x1C: printf("\n"); break;

        case 0x39: printf(" "); break;

        case 0x2A: case 0x36: Shift = true; break;
        case 0xAA: case 0xB6: Shift = false; break;

        case 0x3A: CapsLock = !CapsLock; break;


        default:

            //anything above 0x80 signifies a key lift. we will ignore those for now
            if(key < 0x80){
                //print number of interrupt we didn't handle
                char* foo = "KEY 0x00 ";
                char* hex = "0123456789ABCDF";
                foo[6] = hex[(key >> 4) & 0x0F];
                foo[7] = hex[key & 0x0F];
                printf(foo);
            }
    }

    //XOR shift and CapsLock to see if characters should be uppercase
    if(Shift == CapsLock){
        UpperCase = false;
    }else{
        UpperCase = true;
    }


    

    return esp;
}