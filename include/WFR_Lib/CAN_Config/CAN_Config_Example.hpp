#ifndef __CAN_CONFIG__
#define __CAN_CONFIG__
#include "WFR_Lib/CAN.hpp"
#include "etl/map.h"
#include "etl/set.h"

class M192_Command_Message : public CAN::Message
{
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

    // overridden functions
    MESSAGE_ERROR parse(uint8_t buffer[]) override;
    MESSAGE_ERROR serialize(uint8_t buffer[]) override;

// fields
// the struct is to make parsing a single line operation
#pragma pack(1)
    struct
    {
        int16_t VCU_INV_Torque_Command;
        int16_t VCU_INV_Speed_Command;
        uint8_t VCU_INV_Direction_Command : 1;
        uint8_t padding1 : 7;
        uint8_t VCU_INV_Inverter_Enable : 1;
        uint8_t VCU_INV_Inverter_Discharge : 1;
        uint8_t VCU_INV_Speed_Mode_Enable : 1;
        uint8_t padding2 : 5;
        int16_t VCU_INV_Torque_Limit_Command;
    } raw_signals;

    // access as individual elements:
    CAN::Signal<float> *VCU_INV_Torque_Command;
    CAN::Signal<float> *VCU_INV_Speed_Command;
    CAN::Signal<bool> *VCU_INV_Direction_Command;
    CAN::Signal<bool> *VCU_INV_Inverter_Enable;
    CAN::Signal<bool> *VCU_INV_Inverter_Discharge;
    CAN::Signal<bool> *VCU_INV_Speed_Mode_Enable;
    CAN::Signal<float> *VCU_INV_Torque_Limit_Command;

    std::tuple<CAN::Signal<float>,
               CAN::Signal<float>,
               CAN::Signal<bool>,
               CAN::Signal<bool>,
               CAN::Signal<bool>,
               CAN::Signal<bool>,
               CAN::Signal<float>>
        signals;
};

inline etl::set CAN_Rx_IDs = {192};
inline etl::set CAN_Tx_10ms = {192};
inline etl::set CAN_Tx_100ms = {192};
inline etl::set CAN_Tx_1000ms = {192};
inline etl::map CAN_Map{
    etl::pair{192, M192_Command_Message::Get()},
};

#endif
