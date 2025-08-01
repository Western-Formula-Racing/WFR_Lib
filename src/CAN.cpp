#include "WFR_Lib/CAN.hpp"
#include <stdio.h>

static const char *TAG = "CAN"; // Used for ESP_LOGx commands. See ESP-IDF Documentation


template<class Type>
CAN::Signal<Type>::Signal(float scale, float offset, uint64_t* last_recieved_p, Type default_value): 
scale(scale), offset(offset), last_recieved_p(last_recieved_p), default_value(default_value)
{
}


template<class Type>
Type CAN::Signal<Type>::get(){
    int64_t current_time = esp_timer_get_time()/1000;
    if((current_time - *last_recieved) >= TIMEOUT){
        return default_value;
    }
    return value;
}

template<class Type>
CAN::SIGNAL_ERROR CAN::Signal<Type>::set(Type value){
    value = value;
    return SIGNAL_ERROR::OK;
}



//CAN Base Class Operations
CAN::CAN_ERROR CAN::BaseInterface::begin()
{
    //child class does interface specific setup first 
    rx_sem = xSemaphoreCreateBinary();
    rxTaskHandle = nullptr;
    xSemaphoreGive(rx_sem);
    xTaskCreatePinnedToCore(CAN::BaseInterface::rx_task_wrapper, "CAN_rx", 4096, this, configMAX_PRIORITIES - 20, &CAN::BaseInterface::rxTaskHandle, 1);
}

void CAN::BaseInterface::rx_task_wrapper(void *arg)
{
    CAN::BaseInterface *instance = static_cast<CAN::BaseInterface *>(arg);
    instance->rx_task();
}

void CAN::BaseInterface::tx_CallBack_wrapper(TimerHandle_t xTimer)
{
    CAN::BaseInterface *instance = static_cast<CAN::BaseInterface *>(static_cast<void *>(xTimer));
    instance->tx_CallBack();
}

void CAN::BaseInterface::rx_task()
{
    while (true)
    {
        // Try to take the semaphore without blocking
        if (xSemaphoreTake(rx_sem, 0) == pdTRUE)
        {
            CAN::CanFrame rx_msg;
            
            
            while (CAN::BaseInterface::rx_msg(&rx_msg) == CAN::OK)
            {
                //process message
            }
            xSemaphoreGive(rx_sem);
        }
        else
        {
            // Could not get semaphore, yield
            taskYIELD();
        }
    }
}

void CAN::BaseInterface::tx_CallBack()
{
    txCallBackCounter = (txCallBackCounter < 1000) ? txCallBackCounter + 1 : 0;
    CAN::CanFrame rx_msg;
    // I don't believe in 1ms messages in 2025. nothing's that important

    // 10ms messages

    

    // 100ms messages
    if (txCallBackCounter % 10 == 0)
    {
        //@todo needs an implementation before calling tx
    }
    
    // 1000ms messages
    if (txCallBackCounter % 100 == 0)
    {
        //@todo needs an implementation before calling tx
    }

}