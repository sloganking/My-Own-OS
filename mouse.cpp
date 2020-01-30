#include "mouse.h"

//forward declaration of printf
//so that we can use printf
void printf(char*);

void invertCharAt(uint8_t x, uint8_t y){

    uint16_t* VideoMemory = (uint16_t*)0xb8000;

    VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                            | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                            | (VideoMemory[80*y+x] & 0x00FF);
}

//constructor
MouseDriver::MouseDriver(InterruptManager* manager)
: InterruptHandler(0x2C, manager),      //interrupt number for mouse
dataport(0x60),
commandport(0x64)
{
    offset = 0;
    buttons = 0;

    //at the start flip the character in center of screen (represents mouse)
    invertCharAt(40,12);

    commandport.Write(0xA8); // activate interrupts
    commandport.Write(0x20); // get current state of PIC
    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60); // set current state
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4); // activate
    dataport.Read();
}

//destructor
MouseDriver::~MouseDriver(){

}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp){
    
    uint8_t status = commandport.Read();
    if(!(status & 0x20)){   //test wether there is data
        return esp;
    }

    //initialize mouse position
    static int8_t x = 40, y = 12;

    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3; //move offset

    if(offset == 0){

        //invert colors of old cursor position back to normal
        invertCharAt(x,y);

        //buff 1 == movement on x axis
        x += buffer[1];

        if(x < 0) x = 0;
        if(x >= 80) x = 79;

        y -= buffer[2];
        if(y < 0) y = 0;
        if(y >= 25) y = 24;

        //invert colors at new cursor position
        invertCharAt(x,y);


        for(uint8_t i = 0; i < 3; i++){
            //if buttons are different from last buttons
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01<<i))){
                invertCharAt(x,y);
            }
        }
        buttons = buffer[0];
    }

    return esp;
}