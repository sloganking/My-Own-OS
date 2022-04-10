#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h> 
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <gui/render.h>
#include <multitasking.h>


// #define GRAPHICSMODE

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;



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
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0x0F];
    foo[1] = hex[key & 0x0F];
    printf(foo);
}

void printfHex32(uint32_t key){
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex((key) & 0xFF);
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

void taskA(){
    while(true){
        printf("A");
    }
}

void taskB(){
    while(true){
        printf("B");
    }
}

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
    // printf("Hello World!\n");

    GlobalDescriptorTable gdt;      //initialize Global Descriptor table

    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);

    //10MB
    size_t heap = 10*1024*1024;

    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);

    // printf("heap: 0x");
    // printfHex32((uint32_t)heap);
    // printf("\n");

    void* allocated = memoryManager.malloc(1024);

    void* allocated2 = memoryManager.malloc(255);//239

    memoryManager.free(allocated);

    memoryManager.listChunks();

    TaskManager taskManager;

    // Task task1(&gdt, taskA);
    // Task task2(&gdt, taskB);
    // taskManager.AddTask(&task1);
    // taskManager.AddTask(&task2);

    InterruptManager interrupts(&gdt, &taskManager);  //initialize Interrupt Descriptor table

    printf("Initializing Hardware, Stage 1\n");

    #ifdef GRAPHICSMODE
        Desktop desktop(320,200, 0x00,0x00,0xA8);
    #endif

    DriverManager drvManager;

        //initialize keyboard
        #ifdef GRAPHICSMODE
            KeyboardDriver keyboard(&interrupts, &desktop);
        #else
            PrintfKeyboardEventHandler kbhandler;
            KeyboardDriver keyboard(&interrupts, &kbhandler);
        #endif
        drvManager.AddDriver(&keyboard);

        //initialize mouse
        #ifdef GRAPHICSMODE
            MouseDriver mouse(&interrupts, &desktop);
        #else
            MouseToConsole mousehandler;
            MouseDriver mouse(&interrupts, &mousehandler);
        #endif
        drvManager.AddDriver(&mouse);

        //initialize PCI
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);

        //initialize VGA graphics
        VideoGraphicsArray vga;

        //initialize render frame
        Render rend(320,200);   //arguments do nothing for now. 320,200 is hardcoded into "gui/render.h"

    printf("Initializing Hardware, Stage 2\n");
    drvManager.ActivateAll();


    printf("Initializing Hardware, Stage 3\n");

    #ifdef GRAPHICSMODE
        //screen resolution is 320 px wide, 200 px tall, using 8 bit pixel color depth
        vga.SetMode(320,200,8);

        //make new window and attach it to the desktop
        Window win1(&desktop, 10,10, 20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);

        Window win2(&desktop, 40,15, 30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif

    //tell CPU to allow interrupts
    //should be the last thing in the kernel
    interrupts.Activate();


    //once multitasking has been implemented, this should be moved into a task
    while(1){
        #ifdef GRAPHICSMODE
            //render new frame
            desktop.Draw(&rend);

            //display rendered frame
            rend.display(&vga);
        #endif
    }
}