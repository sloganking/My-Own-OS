#ifndef __MYOS__DRIVERS__DRIVER_H
#define __MYOS__DRIVERS__DRIVER_H

namespace myos{
    namespace drivers{
        class Driver{
            public:
                Driver();
                ~Driver();

                virtual void Activate();    //activation of hardware
                virtual int Reset();   //Resets devices to known starting state (makes sure devices are still communicable after reset or similar)
                virtual void Deactivate();
        };

        class DriverManager{
            private:
                Driver* drivers[255];
                int numDrivers;

            public:
                DriverManager();
                ~DriverManager();
                void AddDriver(Driver*);

                void ActivateAll();
        };
    }
}
#endif