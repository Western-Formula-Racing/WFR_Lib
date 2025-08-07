#include "WFR_Lib/CAN.hpp"
#include <stdio.h>
#include "esp_timer.h"

static const char *TAG = "CAN"; // Used for ESP_LOGx commands. See ESP-IDF Documentation


template<class Type>
CAN::Signal<Type>::Signal(float scale, float offset, uint64_t* last_recieved_p, Type default_value): 
scale(scale), offset(offset), last_recieved_p(last_recieved_p), default_value(default_value)
{
}


template<class Type>
Type CAN::Signal<Type>::get(){
    int64_t current_time = esp_timer_get_time()/1000;
    if((current_time - *last_recieved_p) >= TIMEOUT){
        return default_value;
    }
    return value;
}

template<class Type>
CAN::SIGNAL_ERROR CAN::Signal<Type>::set(Type value){
    value = value;
    return SIGNAL_ERROR::SIGNAL_OK;
}



//CAN Base Class Operations
CAN::CAN_ERROR CAN::BaseInterface::begin()
{
    init();
    rx_sem = xSemaphoreCreateBinary();
    rxTaskHandle = nullptr;
    xSemaphoreGive(rx_sem);
    xTaskCreatePinnedToCore(CAN::BaseInterface::rx_task_wrapper, "CAN_rx", 4096, this, configMAX_PRIORITIES - 20, &CAN::BaseInterface::rxTaskHandle, 1);
    return CAN::CAN_ERROR::OK;
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


//TWAI Interface Implementation

CAN::TWAI_Interface::TWAI_Interface(gpio_num_t CAN_Tx_Pin, gpio_num_t CAN_Rx_Pin, twai_mode_t twai_mode): CAN_Tx_Pin(CAN_Tx_Pin), CAN_Rx_Pin(CAN_Rx_Pin), twai_mode(twai_mode)
{
    
}

CAN::CAN_ERROR CAN::TWAI_Interface::init()
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_Tx_Pin, CAN_Rx_Pin, twai_mode);
    g_config.rx_queue_len = 1000;
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    g_config.controller_id = 0;

    txCallBackCounter = 0;

    if (twai_driver_install_v2(&g_config, &t_config, &f_config, &twai_handle) == ESP_OK)
    {
        ESP_LOGI(TAG, "Driver installed\n");
        
    }
    else
    {
        ESP_LOGI(TAG, "Failed to install driver\n");
        return CAN::CAN_ERROR::TWAI_DRIVER_ERROR;
    }
    if (twai_start_v2(twai_handle) == ESP_OK)
    {
        ESP_LOGI(TAG, "Driver started\n");
        return CAN::CAN_ERROR::OK;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start driver\n");
        return CAN::CAN_ERROR::TWAI_DRIVER_ERROR;
    }

}

const char* CAN::TWAI_Interface::get_twai_error_state_text(twai_status_info_t* status)
{
    if (status->state == TWAI_STATE_STOPPED) return "Stopped";
    else if (status->state == TWAI_STATE_RUNNING) {
        if (status->tx_error_counter >= 128 || status->rx_error_counter >= 128) {
            return "Bus-Off or Error-Passive";
        } else if (status->tx_error_counter > 0 || status->rx_error_counter > 0) {
            return "Error-Active (with some errors)";
        } else {
            return "Active (no errors)";
        }
    } else {
        return "Unknown";
    }
}