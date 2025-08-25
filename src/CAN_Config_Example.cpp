#include "WFR_Lib/CAN.hpp"
#include "WFR_Lib/CAN_Config/CAN_Config.hpp"
static const char *TAG = "CAN_Config"; // Used for ESP_LOGx commands. See ESP-IDF Documentation

M192_Command_Message* M192_Command_Message::instancePtr = nullptr;
SemaphoreHandle_t M192_Command_Message::mutex = xSemaphoreCreateMutex();

M192_Command_Message::M192_Command_Message()
    : signals(
        CAN::Signal<uint16_t>(0,16,0.1,0, &last_recieved),
        CAN::Signal<uint16_t>(16,16,1,0, &last_recieved),
        CAN::Signal<bool>(32,1,1,0, &last_recieved),
        CAN::Signal<bool>(40,1,1,0, &last_recieved),
        CAN::Signal<bool>(41,1,1,0, &last_recieved),
        CAN::Signal<bool>(42,1,1,0, &last_recieved),
        CAN::Signal<uint16_t>(48,16,0.1,0, &last_recieved)
    )
{

    VCU_INV_Torque_Command = &std::get<0>(signals);
    VCU_INV_Speed_Command = &std::get<1>(signals);
    VCU_INV_Direction_Command = &std::get<2>(signals);
    VCU_INV_Inverter_Enable = &std::get<3>(signals);
    VCU_INV_Inverter_Discharge = &std::get<4>(signals);
    VCU_INV_Speed_Mode_Enable = &std::get<5>(signals);
    VCU_INV_Torque_Limit_Command = &std::get<6>(signals);

} 


M192_Command_Message *M192_Command_Message::Get(){
    if (M192_Command_Message::mutex == nullptr) {
        M192_Command_Message::mutex = xSemaphoreCreateMutex();
    }
    if (instancePtr == nullptr && M192_Command_Message::mutex)
    {
        if (xSemaphoreTake(M192_Command_Message::mutex, (TickType_t)10) == pdTRUE)
        {
            instancePtr = new M192_Command_Message();
            xSemaphoreGive(M192_Command_Message::mutex);
        }
        else
        {
            ESP_LOGW(TAG, "Mutex couldn't be obtained");
        }
    }
    return instancePtr;
}

CAN::Message::MESSAGE_ERROR M192_Command_Message::parse(uint8_t buffer[])
{
    memcpy(&raw_signals, buffer, 8*sizeof(uint8_t));
    return OK;
}
CAN::Message::MESSAGE_ERROR M192_Command_Message::serialize(uint8_t buffer[])
{
    memcpy(buffer, &raw_signals, 8*sizeof(uint8_t));
    return OK;
}