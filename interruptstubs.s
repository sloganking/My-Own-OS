.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager15handleInterruptEhj   # compiler given name of InterruptManager::handleInterrupt function in "interrupts.cpp"

.global _ZN16InterruptManager22IgnoreInterruptRequestEv

.macro HandleException num
.global _ZN16InterruptManager16HandleException\num\()Ev
_ZN16InterruptManager16HandleException\num\()Ev:

    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:

    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm


# for more interupts, add more numbers
handleInterruptRequest 0x00
handleInterruptRequest 0x01


int_bottom:

    # store register values
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp
    push (interruptnumber)
    call _ZN16InterruptManager15handleInterruptEhj

    # popOld pointer and interrupt number like this
    # addl $5, %esp
    # don't have to do since stame stack pointer is handed back by "interrupt.cpp" handleInterrupt()

    # do this instead
    movl %eax, %esp

    # recover register values (reverse order from pushing due to how stored on stack)
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

_ZN16InterruptManager22IgnoreInterruptRequestEv:

    iret # tell the processor we are done with interrupt


.data 
    interruptnumber: .byte 0
