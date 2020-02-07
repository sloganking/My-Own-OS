#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h> 
#include <drivers/mouse.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;



//screen 80 chars wide and 25 high
void printf(char* str){
    // pointer to memory location
    // GPU will auto display characters stored at 0xb8000
    uint16_t* VideoMemory = (uint16_t*)0xb8000;

    //cursor variables
    static uint8_t x=0, y=0;

    for(int i = 0; str[i] != '\0'; ++i){

        switch(str[i]){
            case '\n':
                y++;
                x = 0;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        //if behind right border of screen
        if(x >= 80){
            y++;
            x = 0;
        }

        //if reached bottom of screen, clear whole screen
        if(y >= 25){
            //set whole screen to space character
            for(y = 0; y < 25; y++){
                for(x = 0; x < 80; x++){
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                }
            }
            x=0;
            y=0;
        }
    }
}

void printfHex(uint8_t key){
    char* foo = "00 ";
    char* hex = "0123456789ABCDF";
    foo[0] = hex[(key >> 4) & 0x0F];
    foo[1] = hex[key & 0x0F];
    printf(foo);
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler{
    public:
        void OnKeyDown(char c){
            char* foo = " ";
            foo[0] = c;
            printf(foo);
        }
};

void invertCharAt(uint8_t x, uint8_t y){

    uint16_t* VideoMemory = (uint16_t*)0xb8000;

    VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                            | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                            | (VideoMemory[80*y+x] & 0x00FF);
}

class MouseToConsole : public MouseEventHandler {

    int8_t x,y;

public:

    MouseToConsole(){

    }

    virtual void OnActivate(){
        //at the start flip the character in center of screen (represents mouse)
        x = 40;
        y = 12;
        invertCharAt(40,12);
    }

    void OnMouseMove(int xoffset, int yoffset){


        //invert colors of old cursor position back to normal
        invertCharAt(x,y);

        //buff 1 == movement on x axis
        x += xoffset;

        if(x < 0) x = 0;
        if(x >= 80) x = 79;

        y += yoffset;
        if(y < 0) y = 0;
        if(y >= 25) y = 24;

        //invert colors at new cursor position
        invertCharAt(x,y);

    }
};

// define what constructor means
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

//iterates over space between start_ctors and end_ctors and jumps into all function pointers
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++){
        //envoce constructor calls
        (*i)();
    }
}

//paramaters are data retrieved from bootloader
extern "C" void kernelMain(void* multiboot_structure, uint32_t magicnumber){
    printf("Hello World!\n");

    GlobalDescriptorTable gdt;      //initialize Global Descriptor table
    InterruptManager interrupts(&gdt);  //initialize Interrupt Descriptor table

    printf("Initializing Hardware, Stage 1\n");
    
    DriverManager drvManager;

        PrintfKeyboardEventHandler kbhandler;
        KeyboardDriver keyboard(&interrupts, &kbhandler);
        drvManager.AddDriver(&keyboard);
        
        MouseToConsole mousehandler;
        MouseDriver mouse(&interrupts, &mousehandler);
        drvManager.AddDriver(&mouse);

        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager);

        printf("Initializing Hardware, Stage 2\n");
        drvManager.ActivateAll();


    printf("Initializing Hardware, Stage 3\n");
    interrupts.Activate();  //tell CPU to allow interrupts

    while(1);   // so that the kernel doesn't stop
}