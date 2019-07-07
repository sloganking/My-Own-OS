.section .text
.extern kernelMain  ;tell assembler that there is going to be something called kernelMain
.global loader

loader:
    mov $kernel_stack, $esp
    call kernelMain

_stop:
    cli
    hlt
    jmp _stop


.section .bss
.space 2*1024*1024;     # 2 MiB
kernel_stack: