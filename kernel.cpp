#include "types.h"
#include "gdt.h"

void printf(char* str){
    // pointer to memory location
    // GPU will auto display characters stored at 0xb8000
    uint16_t* VideoMemory = (uint16_t*)0xb8000;

    for(int i = 0; str[i] != '\0'; ++i){
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
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
    printf("Hello World!");

    GlobalDescriptorTable gdt;

    while(1);   // so that the kernel doesn't stop
}