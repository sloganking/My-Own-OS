#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "driver.h"
#include "keyboard.h"
#include "mouse.h"


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
    
    // while(1){
    //     for(uint32_t i=0; i<100000000; i++){        //wait some time
    //         printf("");
    //     }
    //     printf("Count...\n");
    // }

    GlobalDescriptorTable gdt;      //initialize Global Descriptor table
    InterruptManager interrupts(&gdt);  //initialize Interrupt Descriptor table

    printf("Initializing Hardware, Stage 1\n");
    
    DriverManager drvManager;
    
    KeyboardDriver keyboard(&interrupts);
    drvManager.AddDriver(&keyboard);
    
    MouseDriver mouse(&interrupts);
    drvManager.AddDriver(&mouse);

    printf("Initializing Hardware, Stage 2\n");
    drvManager.ActivateAll();


    printf("Initializing Hardware, Stage 3\n");
    interrupts.Activate();  //tell CPU to allow interrupts

    while(1);   // so that the kernel doesn't stop
}