.set IRQ_BASE, 0x20

.section .text

.extern _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj   # compiler given name of InterruptManager::handleInterrupt function in "interrupts.cpp"

.global _ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv

.macro HandleException num
.global _ZN4myos21hardwarecommunication16InterruptManager16HandleException\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager16HandleException\num\()Ev:

    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:

    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0    # for error field
    jmp int_bottom
.endm


# for more interupts, add more numbers
handleInterruptRequest 0x00
handleInterruptRequest 0x01
handleInterruptRequest 0x0C


int_bottom:

    # store register values
        # pusha
        # pushl %ds
        # pushl %es
        # pushl %fs
        # pushl %gs

        pushl %ebp
        pushl %edi
        pushl %esi

        pushl %edx
        pushl %ecx
        pushl %ebx
        pushl %eax

    # pass function arguments
    pushl %esp
    push (interruptnumber)

    # call C++ handler
    call _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj

    # popOld pointer and interrupt number like this
    # addl $5, %esp
    # don't have to do since stame stack pointer is handed back by "interrupt.cpp" handleInterrupt()

    # do this instead
    mov %eax, %esp # switch the stack

    # recover register values (reverse order from pushing due to how stored on stack)
        popl %eax
        popl %ebx
        popl %ecx
        popl %edx
        
        popl %esi
        popl %edi
        popl %ebp

        # popl %gs
        # popl %fs
        # popl %es
        # popl %ds
        # popa

    add $4, %esp    # POP error field

_ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv:

    # tell the processor we are done with interrupt
    iret


.data 
    interruptnumber: .byte 0
