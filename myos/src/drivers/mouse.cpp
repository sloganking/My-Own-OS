#include <drivers/mouse.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

//forward declaration of printf
//so that we can use printf
void printf(char*);

//constructor
MouseEventHandler::MouseEventHandler(){

}

void MouseEventHandler::OnActivate(){

}
void MouseEventHandler::OnMouseDown(uint8_t button){

}
void MouseEventHandler::OnMouseUp(uint8_t button){

}
void MouseEventHandler::OnMouseMove(int x, int y){

}

//constructor
MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(0x2C, manager),      //interrupt number for mouse
dataport(0x60),
commandport(0x64)
{
    this->handler = handler;
}

//destructor
MouseDriver::~MouseDriver(){

}

void MouseDriver::Activate(){
    offset = 0;
    buttons = 0;

    if(handler != 0){
        handler->OnActivate();
    }

    commandport.Write(0xA8); // activate interrupts
    commandport.Write(0x20); // get current state of PIC
    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60); // set current state
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4); // activate
    dataport.Read();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp){

    uint8_t status = commandport.Read();
    if(!(status & 0x20)){   //if (there is no data) or (no mousehandler) then just return without doing anything
        return esp;
    }

    //initialize mouse position
    static int8_t x = 40, y = 12;

    buffer[offset] = dataport.Read();

    if(handler == 0){   //if (no mousehandler) then just return without doing anything
        return esp;
    }

    offset = (offset + 1) % 3; //move offset

    //finished recieving data from mouse
    if(offset == 0){
        //if mouse moved in X or Y axis
        if(buffer[1] != 0 || buffer[2] != 0){
            handler->OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
        }


        for(uint8_t i = 0; i < 3; i++){
            //if buttons are different from last buttons
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01<<i))){
                if(buttons & (0x1<<i)){
                    handler->OnMouseUp(i+1);
                }else{
                    handler->OnMouseDown(i+1);
                }
                // invertCharAt(x,y);
            }
        }
        buttons = buffer[0];
    }

    return esp;
}