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
void PeripheralComponentInterconnectController::SelectDrivers(myos::drivers::DriverManager* driverManager){
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
                    break;  //nothing to give a driver and no more functions after this one
                }

                printf("PCI BUS ");
                printfHex(bus & 0xFF);

                printf(", DEVICE ");
                printfHex(device & 0xFF);

                printf(", FUNCTION ");
                printfHex(function & 0xFF);

                printf("= VENDOR ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex(dev.vendor_id & 0xFF);

                printf(", DEVICE ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);

                printf("\n");

            }
        }
    }
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