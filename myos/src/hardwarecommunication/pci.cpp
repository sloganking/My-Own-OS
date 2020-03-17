#include <hardwarecommunication/pci.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char* str);
void printfHex(uint8_t);

//contstructor
PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor(){

}

//destructor
PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor(){

}

//contstructor
PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
:   dataPort(0xCFC),
    commandPort(0xCF8)
{

}

//destructor
PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController(){

}



uint32_t PeripheralComponentInterconnectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset){

    //construct identifier to send to the PCI controller
    uint32_t id = 
        0x1 << 31       //first bit is always a 1
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);
    commandPort.Write(id);
    uint32_t result = dataPort.Read();  //grab alligned 32 bit integer
    return result >> (8* registeroffset % 4);    //return only the requested byte
}



void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value){

    uint32_t id = 
        0x1 << 31       //first bit is always a 1
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);
    commandPort.Write(id);
    dataPort.Write(value);
}


bool PeripheralComponentInterconnectController::DeviceHasFunctions(uint16_t bus, uint16_t device){

    //7nth bit of the first function's 0x0E address tells us if the device has functions or not
    return Read(bus, device, 0, 0x0E) & (1<<7);
}


//enumerate busses, devices and functions and select drivers based on information about (class_id, subclass_id, vendor_id, device_id) before putting drivers into the kernel's driver manager
void PeripheralComponentInterconnectController::SelectDrivers(myos::drivers::DriverManager* driverManager, myos::hardwarecommunication::InterruptManager* interrupts){
    //iterate over busses
    for(int bus = 0; bus < 8; bus++){
        //iterate over devices
        for(int device = 0; device < 32; device++){

            //if device has functions, numFunctions = 8, else numFunctions = 1
            int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;

            //iterate over functions
            for(int function = 0; function < numFunctions; function++){
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);

                //vendor ID will be all 0s or 1s if there is no device on the function.
                //if there is no vendor_id
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF){
                    continue;  //no function here so skip this one
                }

                //iterate over base Address Registers
                for(int barNum = 0; barNum < 6; barNum++){
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);

                    //only proceed if BAR is really set and is Input/Outpot type.
                    if(bar.address && (bar.type == InputOutput)){
                        dev.portBase = (uint32_t)bar.address;
                    }

                    //grab driver for device and connect it to the interrupt manager
                    Driver* driver = GetDriver(dev, interrupts);

                    //if there was a driver, add it to the driver manager.
                    if(driver != 0){
                        driverManager->AddDriver(driver);
                    }
                }

                // printf("PCI BUS ");
                // printfHex(bus & 0xFF);

                // printf(", DEVICE ");
                // printfHex(device & 0xFF);

                // printf(", FUNCTION ");
                // printfHex(function & 0xFF);

                // printf("= VENDOR ");
                // printfHex((dev.vendor_id & 0xFF00) >> 8);
                // printfHex(dev.vendor_id & 0xFF);

                // printf(", DEVICE ");
                // printfHex((dev.device_id & 0xFF00) >> 8);
                // printfHex(dev.device_id & 0xFF);

                // printf("\n");

            }
        }
    }
}


BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar){
    BaseAddressRegister result;

    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;

    //if 64 bit, the are only 2 BAR instead of the normal 6
    int maxBARs = 6 - (4*headertype);

    //return null for any BAR after the second one
    //"result" has not been set to anything yet so it contains null
    if(bar >= maxBARs){
        return result;
    }

    //base address registers have a size of 4
    //0x10 is the offset to the first one
    uint32_t bar_value = Read(bus, device, function, 0x10 + 4*bar);

    //examine the last bit of BAR which tells us if it's Input/Output type or a MemoryMapping type.
    //if it's one, then it's InputOutput, else it's MemoryMapping
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;

    uint32_t temp;

    if(result.type == MemoryMapping){

        result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;

        switch((bar_value) >> 1 & 0x3){
            case 0: //32 bit mode
            case 1: //20 bit mode
            case 2: //64 bit mode
            break;
        }

    }else{
        //I think ~0x3 is the same as 0xFFFC, which would chop off the last two bits
        result.address = (uint8_t* )(bar_value & ~0x3);
        result.prefetchable = false;
    }

    return result;
}    

Driver* PeripheralComponentInterconnectController::GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev, InterruptManager* interrupts){
    //normally one might store a list of drivers on a HDD and retrieve the desired one. Since we don't yet have HDD access as of writing this, I will hard code the drivers into the kernel

    switch(dev.vendor_id){
        case 0x1022:    //AMD
            switch(dev.device_id){
                case 0x2000:    //am79c973
                    break;
            }
        break;

        case 0x8086:    //intel
        break;
    }

    //if we don't have a driver for specific device (as searched for above), then we may give driver for generic devices
    switch(dev.class_id){
        case 0x3:   //graphics
            switch(dev.subclass_id){
                case 0x00:  //VGA
                break;
            }
        break;
    }

    return 0;
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function){
    PeripheralComponentInterconnectDeviceDescriptor result;

    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0B);
    result.subclass_id = Read(bus, device, function, 0x0A);
    result.interface_id = Read(bus, device, function, 0x09);

    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3C);

    return result;
}