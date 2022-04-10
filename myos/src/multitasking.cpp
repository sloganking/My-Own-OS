#include <multitasking.h>

using namespace myos;
using namespace myos::common;

//> class Task{

    //> public:

        //constructor
        Task::Task(GlobalDescriptorTable *gdt, void entrypoint()){

            //starting cpu state should be the end of the stack so.
            //pointer to (start of stack) + (size of stack) - sizeof(CPUState)
            cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));

            cpustate -> eax = 0;
            cpustate -> ebx = 0;
            cpustate -> ecx = 0;
            cpustate -> edx = 0;

            cpustate -> esi = 0;
            cpustate -> edi = 0;
            cpustate -> ebp = 0;

            // cpustate -> gs = 0;
            // cpustate -> fs = 0;
            // cpustate -> es = 0;
            // cpustate -> ds = 0;

            // cpustate -> error = 0;

            //>only has to do with userspace and different security levels
                // cpustate -> esp = ;
                // cpustate -> ss = 0;
            //<

            cpustate -> eip = (uint32_t)entrypoint;
            cpustate -> cs = gdt->CodeSegmentSelector();
            cpustate -> eflags = 0x202;

        }

        //destructor
        Task::~Task(){

        }
    //<

//<> class TaskManager{

    //> public:

        //constructor
        TaskManager::TaskManager(){
            numTasks = 0;
            currentTask = -1;
        }

        //destructor
        TaskManager::~TaskManager(){

        }

        bool TaskManager::AddTask(Task* task){
            if(numTasks >= 256){
                return false;
            }
            tasks[numTasks++] = task;
            return true;
        }

        CPUState* TaskManager::Schedule(CPUState* cpustate){

            if(numTasks <= 0){
                return cpustate;    //resume previous task
            }

            if(currentTask >= 0){

                //store old cpu state
                tasks[currentTask]->cpustate = cpustate;
            }

            //round robin scheduling
            //if current task exceeds number of tasks, start over at beginning
            if(++currentTask >= numTasks){
                currentTask %= numTasks;
            }
            return tasks[currentTask]->cpustate;
        }
    //<
//<
