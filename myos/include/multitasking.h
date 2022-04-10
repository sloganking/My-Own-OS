#ifndef __MYOS__MULTITASKING_h
#define __MYOS__MULTITASKING_h

#include <common/types.h>
#include <gdt.h>

namespace myos{

    struct CPUState{

        //>we have to manually push these

            //general purpose registers
            common::uint32_t eax;
            common::uint32_t ebx;
            common::uint32_t ecx;
            common::uint32_t edx;

            common::uint32_t esi;   //stack index
            common::uint32_t edi;   //data index
            common::uint32_t ebp;   //base pointer

            // common::uint32_t gs;
            // common::uint32_t fs;
            // common::uint32_t es;
            // common::uint32_t ds;

            common::uint32_t error;

        //<>automatically pushed by the processor
            common::uint32_t eip;   //instruction pointer
            common::uint32_t cs;    //code segment
            common::uint32_t eflags;    //flags
            common::uint32_t esp;   //stack pointer
            common::uint32_t ss;    //stack segment
        //<

    } __attribute__((packed));

    class Task{
        friend class TaskManager;
        private:

            //4kB of stack memory
            common::uint8_t stack[4096];    

            //cpu pushed and user pushed data
            CPUState* cpustate;

        public:
            //constructor
            Task(GlobalDescriptorTable *gdt, void entrypoint());

            //destructor
            ~Task();
    };

    class TaskManager{
        private:
            Task* tasks[256];
            int numTasks;
            int currentTask;
        public:
            //constructor
            TaskManager();

            //destructor
            ~TaskManager();
            bool AddTask(Task* task);
            CPUState* Schedule(CPUState* cpustate);
    };
}

#endif