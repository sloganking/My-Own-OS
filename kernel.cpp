//paramaters are data retrieved from bootloader
void kernelMain(void* multiboot_structure, unsigned int magicnumber){
    printf("Hello World!");

    while(1);   // so that the kernel doesn't stop
}