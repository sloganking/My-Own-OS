#include <drivers/driver.h>

using namespace myos::drivers;


Driver::Driver(){

}
Driver::~Driver(){

}

//activation of hardware
void Driver::Activate(){

}

//Resets devices to known starting state (makes sure devices are still communicable after reset or similar)
int Driver::Reset(){

}

void Driver::Deactivate(){

}



//constructor
DriverManager::DriverManager(){

}

//destructor
DriverManager::~DriverManager(){

}

void DriverManager::AddDriver(Driver* drv){
    drivers[numDrivers] = drv;
    numDrivers++;
}

void DriverManager::ActivateAll(){

    for(int i = 0; i < numDrivers; i++){
        drivers[i]->Activate();
    }
}