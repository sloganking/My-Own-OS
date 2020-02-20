# set magic number
# used to tell bootloader that this is a bootable operating system
.set MAGIC, 0x1badb002 
# set flags for bootloader
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

# tell compiler to put these in .o file
.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

# set stack pointer (boot loader didn't set it)
.section .text

# tell assembler that there is going to be something called kernelMain
.extern kernelMain 

# tell "loader.s" there is a function called callConstructors
.extern callConstructors
.global loader

# program entry point
loader:
    # set stack pointer
    # $esp = stack pointer
    mov $kernel_stack, %esp

    call callConstructors

    # get pointer to info from bootloader and stores in kernel stack
    push %eax
    push %ebx

    # run the kernel
    call kernelMain     # never suppose to leave

_stop:      # just incase program exits kernelMain
    cli
    hlt
    jmp _stop


.section .bss
.space 2*1024*1024 #      2 MiB of empty space
kernel_stack:
