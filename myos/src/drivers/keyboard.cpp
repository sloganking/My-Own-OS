#include <drivers/keyboard.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

//forward declaration of printf
//so that we can use printf
void printf(char*);
void printfHex(uint8_t);


KeyboardEventHandler::KeyboardEventHandler (){

}


//the default implementation just ignors these events
void KeyboardEventHandler::OnKeyDown(char){
}
void KeyboardEventHandler::OnKeyUp(char){
}


//constructor
KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler *handler)
: InterruptHandler(0x21, manager),
dataport(0x60),
commandport(0x64)
{
    this->handler = handler;
}

//destructor
KeyboardDriver::~KeyboardDriver(){

}

void KeyboardDriver::Activate(){
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

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){

    //fetch the pressed key
    uint8_t key = dataport.Read();

    if(handler == 0){
        return esp;
    }

    static bool Shift = false;
    static bool CapsLock = false;
    static bool UpperCase = false;

    switch(key){
        case 0x02: if(Shift) handler->OnKeyDown('!'); else handler->OnKeyDown('1'); break;
        case 0x03: if(Shift) handler->OnKeyDown('@'); else handler->OnKeyDown('2'); break;
        case 0x04: if(Shift) handler->OnKeyDown('#'); else handler->OnKeyDown('3'); break;
        case 0x05: if(Shift) handler->OnKeyDown('$'); else handler->OnKeyDown('4'); break;
        case 0x06: if(Shift) handler->OnKeyDown('%'); else handler->OnKeyDown('5'); break;
        case 0x07: if(Shift) handler->OnKeyDown('^'); else handler->OnKeyDown('6'); break;
        case 0x08: if(Shift) handler->OnKeyDown('&'); else handler->OnKeyDown('7'); break;
        case 0x09: if(Shift) handler->OnKeyDown('*'); else handler->OnKeyDown('8'); break;
        case 0x0A: if(Shift) handler->OnKeyDown('('); else handler->OnKeyDown('9'); break;
        case 0x0B: if(Shift) handler->OnKeyDown(')'); else handler->OnKeyDown('0'); break;

        case 0x10: if(UpperCase) handler->OnKeyDown('Q'); else handler->OnKeyDown('q'); break;
        case 0x11: if(UpperCase) handler->OnKeyDown('W'); else handler->OnKeyDown('w'); break;
        case 0x12: if(UpperCase) handler->OnKeyDown('E'); else handler->OnKeyDown('e'); break;
        case 0x13: if(UpperCase) handler->OnKeyDown('R'); else handler->OnKeyDown('r'); break;
        case 0x14: if(UpperCase) handler->OnKeyDown('T'); else handler->OnKeyDown('t'); break;
        case 0x15: if(UpperCase) handler->OnKeyDown('Y'); else handler->OnKeyDown('y'); break;
        case 0x16: if(UpperCase) handler->OnKeyDown('U'); else handler->OnKeyDown('u'); break;
        case 0x17: if(UpperCase) handler->OnKeyDown('I'); else handler->OnKeyDown('i'); break;
        case 0x18: if(UpperCase) handler->OnKeyDown('O'); else handler->OnKeyDown('o'); break;
        case 0x19: if(UpperCase) handler->OnKeyDown('P'); else handler->OnKeyDown('p'); break;

        case 0x1E: if(UpperCase) handler->OnKeyDown('A'); else handler->OnKeyDown('a'); break;
        case 0x1F: if(UpperCase) handler->OnKeyDown('S'); else handler->OnKeyDown('s'); break;
        case 0x20: if(UpperCase) handler->OnKeyDown('D'); else handler->OnKeyDown('d'); break;
        case 0x21: if(UpperCase) handler->OnKeyDown('F'); else handler->OnKeyDown('f'); break;
        case 0x22: if(UpperCase) handler->OnKeyDown('G'); else handler->OnKeyDown('g'); break;
        case 0x23: if(UpperCase) handler->OnKeyDown('H'); else handler->OnKeyDown('h'); break;
        case 0x24: if(UpperCase) handler->OnKeyDown('J'); else handler->OnKeyDown('j'); break;
        case 0x25: if(UpperCase) handler->OnKeyDown('K'); else handler->OnKeyDown('k'); break;
        case 0x26: if(UpperCase) handler->OnKeyDown('L'); else handler->OnKeyDown('l'); break;
        case 0x27: if(Shift) handler->OnKeyDown(':'); else handler->OnKeyDown(';'); break;

        case 0x2C: if(UpperCase) handler->OnKeyDown('Z'); else handler->OnKeyDown('z'); break;
        case 0x2D: if(UpperCase) handler->OnKeyDown('X'); else handler->OnKeyDown('x'); break;
        case 0x2E: if(UpperCase) handler->OnKeyDown('C'); else handler->OnKeyDown('c'); break;
        case 0x2F: if(UpperCase) handler->OnKeyDown('V'); else handler->OnKeyDown('v'); break;
        case 0x30: if(UpperCase) handler->OnKeyDown('B'); else handler->OnKeyDown('b'); break;
        case 0x31: if(UpperCase) handler->OnKeyDown('N'); else handler->OnKeyDown('n'); break;
        case 0x32: if(UpperCase) handler->OnKeyDown('M'); else handler->OnKeyDown('m'); break;
        case 0x33: if(Shift) handler->OnKeyDown('<'); else handler->OnKeyDown(','); break;
        case 0x34: if(Shift) handler->OnKeyDown('>'); else handler->OnKeyDown('.'); break;
        case 0x35: if(Shift) handler->OnKeyDown('_'); else handler->OnKeyDown('-'); break;

        case 0x1A: if(Shift) handler->OnKeyDown('{'); else handler->OnKeyDown('['); break;
        case 0x1B: if(Shift) handler->OnKeyDown('}'); else handler->OnKeyDown(']'); break;
        case 0x1C: handler->OnKeyDown('\n'); break;

        case 0x39: handler->OnKeyDown(' '); break;

        case 0x2A: case 0x36: Shift = true; break;
        case 0xAA: case 0xB6: Shift = false; break;

        case 0x3A: CapsLock = !CapsLock; break;


        default:

            //anything above 0x80 signifies a key lift. we will ignore those for now
            if(key < 0x80){

                printf("KEY: 0x");
                printfHex(key);
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