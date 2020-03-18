#ifndef __MYOS__DRIVERS__AMD_AM79C973_H
#define __MYOS__DRIVERS__AMD_AM79C973_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/pci.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>


namespace myos{
    namespace drivers{

        class amd_am79c973 : public Driver, public hardwarecommunication::InterruptHandler{

            struct InitializationBlock{
                common::uint16_t mode;

                //4 reserved bits
                unsigned reserved1 : 4;

                // number of receive and send buffers
                unsigned numSendBuffers : 4;
                unsigned reserved2 : 4;
                unsigned numRecvBuffers : 4;

                common::uint64_t physicalAddress : 48;  //MAC Address
                common::uint16_t reserved3;
                common::uint64_t logicalAddress;    // ??? might be IP address
                common::uint32_t recvBufferDescrAddress;
                common::uint32_t sendBufferDescrAddress;

            };  __attribute__((packed));

            struct BufferDescriptor{
                common::uint32_t address;
                common::uint32_t flags;
                common::uint32_t flags2;
                common::uint32_t avail;

            }; __attribute__((packed));

            hardwarecommunication::Port16Bit MACAddress0Port;
            hardwarecommunication::Port16Bit MACAddress2Port;
            hardwarecommunication::Port16Bit MACAddress4Port;

            hardwarecommunication::Port16Bit registerDataPort;
            hardwarecommunication::Port16Bit registerAddressPort;
            hardwarecommunication::Port16Bit resetPort;
            hardwarecommunication::Port16Bit busControlRegisterDataPort;

            //structure initialization block
            //holds pointer to the array of BufferDescriptors, which hold pointers to the buffers
                InitializationBlock initBlock;

                BufferDescriptor* sendBufferDescr;
                common::uint8_t sendBufferDescrMemory[2048 + 15];
                //8 buffers of size 2KB plus 15 for allignment
                common::uint8_t sendBuffers[2*1024+15][8];
                common::uint8_t currentSendBuffer;

                BufferDescriptor* recvBufferDescr;
                common::uint8_t recvBufferDescrMemory[2048 + 15];
                common::uint8_t recvBuffers[2*1024+15][8];
                common::uint8_t currentRecvBuffer;

            public:
                amd_am79c973(hardwarecommunication::PeripheralComponentInterconnectDeviceDescriptor dev, hardwarecommunication::InterruptManager* interrupts);
                ~amd_am79c973();

                void Activate();
                int Reset();
                common::uint32_t HandleInterrupt(common::uint32_t esp);
        };
    }
}
#endif