#ifndef __CAN_CONFIG__
#define __CAN_CONFIG__
#include "WFR_Lib/CAN.hpp"
#include "etl/map.h"

class M192_Command_Message: public CAN::Message{
    private:
        // Singleton device class structure
        static M192_Command_Message *instancePtr;
        static SemaphoreHandle_t mutex;
        M192_Command_Message();    
    public:
        // Deleting the copy constructor and copy reference constructor to prevent copies
        M192_Command_Message(const M192_Command_Message &) = delete;
        M192_Command_Message &operator=(const M192_Command_Message &) = delete;
        M192_Command_Message(M192_Command_Message &&) = delete;
        M192_Command_Message &operator=(M192_Command_Message &&) = delete;
        static M192_Command_Message *Get();

        //overridden functions
        MESSAGE_ERROR parse(uint8_t buffer[]) override;
        MESSAGE_ERROR serialize(uint8_t buffer[]) override;

        //fields
        //the struct is to make parsing a single line operation
        #pragma pack(1)
        struct
        {
            uint16_t VCU_INV_Torque_Command;
            uint16_t VCU_INV_Speed_Command;
            uint8_t VCU_INV_Direction_Command: 1;
            uint8_t VCU_INV_Inverter_Enable: 1;
            uint8_t padding1: 7;
            uint8_t VCU_INV_Inverter_Discharge: 1;
            uint8_t VCU_INV_Speed_Mode_Enable: 1;
            uint8_t padding2: 5;
            uint16_t VCU_INV_Torque_Limit_Command; 
        }raw_signals;

        //access as individual elements:
        CAN::Signal<uint16_t>* VCU_INV_Torque_Command;
        CAN::Signal<uint16_t>* VCU_INV_Speed_Command;
        CAN::Signal<bool>* VCU_INV_Direction_Command;
        CAN::Signal<bool>* VCU_INV_Inverter_Enable;
        CAN::Signal<bool>* VCU_INV_Inverter_Discharge;
        CAN::Signal<bool>* VCU_INV_Speed_Mode_Enable;
        CAN::Signal<uint16_t>* VCU_INV_Torque_Limit_Command;

        
        std::tuple< CAN::Signal<uint16_t>,
                    CAN::Signal<uint16_t>,
                    CAN::Signal<bool>,
                    CAN::Signal<bool>,
                    CAN::Signal<bool>,
                    CAN::Signal<bool>,
                    CAN::Signal<uint16_t>> signals;

    };




inline etl::map CAN_Rx_Map{
    etl::pair{192, M192_Command_Message::Get()},
    };


#endif
