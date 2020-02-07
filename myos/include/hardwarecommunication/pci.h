#ifndef __MYOS__HARDWARECOMMUNICTION__PCI_H
#define __MYOS__HARDWARECOMMUNICTION__PCI_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>

namespace myos{
    namespace hardwarecommunication{

        //stores data about PCI device functions 
        class PeripheralComponentInterconnectDeviceDescriptor{
            public:
                myos::common::uint32_t portBase;  //port used for communication, number we get from PCI controller
                myos::common::uint32_t interrupt;

                myos::common::uint16_t bus;
                myos::common::uint16_t device;
                myos::common::uint16_t function;

                myos::common::uint16_t vendor_id;
                myos::common::uint16_t device_id;

                myos::common::uint8_t class_id;
                myos::common::uint8_t subclass_id;
                myos::common::uint8_t interface_id;

                myos::common::uint8_t revision;

                PeripheralComponentInterconnectDeviceDescriptor();
                ~PeripheralComponentInterconnectDeviceDescriptor();

        };

        class PeripheralComponentInterconnectController{

            Port32Bit dataPort;
            Port32Bit commandPort;

            public:
                PeripheralComponentInterconnectController();
                ~PeripheralComponentInterconnectController();

                //functions for reading and writing to PCI functions
                myos::common::uint32_t Read(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function, myos::common::uint32_t registeroffset);
                void Write(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function, myos::common::uint32_t registeroffset, myos::common::uint32_t value);
                bool DeviceHasFunctions(myos::common::uint16_t bus, myos::common::uint16_t device);

                //enumerate busses, devices and functions and select drivers based on information about (class_id, subclass_id, vendor_id, device_id) before putting drivers into the kernel's driver manager
                void SelectDrivers(myos::drivers::DriverManager* driverManager);

                PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function);
        };
    }
}

#endif