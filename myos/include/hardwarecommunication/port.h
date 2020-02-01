#ifndef __MYOS__HARDWARECOMMUNICTION__PORT_H
#define __MYOS__HARDWARECOMMUNICTION__PORT_H

#include <common/types.h>

namespace myos{
    namespace hardwarecommunication{
        class Port{
            protected:
                myos::common::uint16_t portnumber;
                Port(myos::common::uint16_t portnumber);  //constructor
                ~Port();     //Destructor
        };

        class Port8Bit : public Port{
            public:
                Port8Bit(myos::common::uint16_t portnumber);  //constructor
                ~Port8Bit();     //Destructor
                virtual void Write(myos::common::uint8_t data);
                virtual myos::common::uint8_t Read();
        };

        class Port8BitSlow : public Port8Bit{
            public:
                Port8BitSlow(myos::common::uint16_t portnumber);  //constructor
                ~Port8BitSlow();     //Destructor
                virtual void Write(myos::common::uint8_t data);
        };

        class Port16Bit : public Port{
            public:
                Port16Bit(myos::common::uint16_t portnumber);  //constructor
                ~Port16Bit();     //Destructor
                virtual void Write(myos::common::uint16_t data);
                virtual myos::common::uint16_t Read();
        };

        class Port32Bit : public Port{
            public:
                Port32Bit(myos::common::uint16_t portnumber);  //constructor
                ~Port32Bit();     //Destructor
                virtual void Write(myos::common::uint32_t data);
                virtual myos::common::uint32_t Read();
        };
    }
}
#endif