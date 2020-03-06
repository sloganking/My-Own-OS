#ifndef __MYOS__MULTITASKING_h
#define __MYOS__MULTITASKING_h

namespace myos{
    class Task{
        friend class TaskManager;
        private:

            //4kB of stack memory
            common::uint8_t stack[4096];    

            //cpu pushed and user pushed data
            CPUState* cpustate

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
            CPUState* Schedule(CPUstate* cpustate);
    };
}

#endif