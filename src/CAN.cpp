#include "WFR_Lib/CAN.hpp"
#include <stdio.h>
// change this later
#include "WFR_Lib/CAN_Config/CAN_Config_Example.hpp"

static const char *TAG = "CAN"; // Used for ESP_LOGx commands. See ESP-IDF Documentation
using namespace CAN;



//CAN Base Class Operations
CAN_ERROR BaseInterface::begin()
{
    init();
    rx_sem = xSemaphoreCreateBinary();
    rxTaskHandle = nullptr;
    xSemaphoreGive(rx_sem);
    xTaskCreatePinnedToCore(BaseInterface::rx_task_wrapper, "CAN_rx", 4096, this, configMAX_PRIORITIES - 20, &rxTaskHandle, 1);
    return CAN_ERROR::OK;
}

void BaseInterface::rx_task_wrapper(void *arg)
{
    BaseInterface *instance = static_cast<BaseInterface *>(arg);
    instance->rx_task();
}

void BaseInterface::tx_CallBack_wrapper(TimerHandle_t xTimer)
{
    BaseInterface *instance = static_cast<BaseInterface *>(static_cast<void *>(xTimer));
    instance->tx_CallBack();
}

void BaseInterface::rx_task()
{
    while (true)
    {
        // Try to take the semaphore without blocking
        if (xSemaphoreTake(rx_sem, 0) == pdTRUE)
        {
            CanFrame rx_msg;
            
            
            while (this->rx_msg(&rx_msg) == OK)
            {
                if (CAN_Rx_Map.find(rx_msg.id) != CAN_Rx_Map.end())
                {
                    printf("address of message: 0x%08x\n", (uint) CAN_Rx_Map[rx_msg.id]);
                    CAN_Rx_Map[rx_msg.id]->parse(rx_msg.buffer);
                }
                
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

void BaseInterface::tx_CallBack()
{
    txCallBackCounter = (txCallBackCounter < 1000) ? txCallBackCounter + 1 : 0;
    CanFrame rx_msg;
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

TWAI_Interface::TWAI_Interface(gpio_num_t CAN_Tx_Pin, gpio_num_t CAN_Rx_Pin, twai_mode_t twai_mode): CAN_Tx_Pin(CAN_Tx_Pin), CAN_Rx_Pin(CAN_Rx_Pin), twai_mode(twai_mode)
{
    
}

CAN_ERROR TWAI_Interface::init()
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
        return CAN_ERROR::TWAI_DRIVER_ERROR;
    }
    if (twai_start_v2(twai_handle) == ESP_OK)
    {
        ESP_LOGI(TAG, "Driver started\n");
        return CAN_ERROR::OK;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start driver\n");
        return CAN_ERROR::TWAI_DRIVER_ERROR;
    }

}

const char* TWAI_Interface::get_twai_error_state_text(twai_status_info_t* status)
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

CAN_ERROR TWAI_Interface::rx_msg(CAN::CanFrame* can_frame)
{
    twai_message_t rx_msg;
    uint32_t twai_alerts;
    twai_read_alerts_v2(twai_handle, &twai_alerts, 0);
    if (twai_alerts & TWAI_ALERT_RX_QUEUE_FULL)
    {
        ESP_LOGW(TAG, "rx queue full: msg dropped");
    }
    if(twai_receive(&rx_msg, portMAX_DELAY) == ESP_OK){
        can_frame->id = rx_msg.identifier;
        can_frame->dlc = rx_msg.data_length_code;
        can_frame->extd = rx_msg.extd;
        for (int i = 0; i < rx_msg.data_length_code; i++)
        {
            can_frame->buffer[i] = rx_msg.data[i];
        }
        return OK;
    }
    return RX_QUEUE_EMPTY;
}

CAN_ERROR TWAI_Interface::tx_msg(CAN::CanFrame* can_frame)
{
    twai_message_t tx_msg;
    tx_msg.identifier = can_frame->id;
    tx_msg.data_length_code = can_frame->dlc;
    tx_msg.extd = can_frame->extd;
    for (int i = 0; i < tx_msg.data_length_code; i++){
        tx_msg.data[i] = can_frame->buffer[i];
    }
    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(1000)) != ESP_OK)
    {
        return TX_ERROR;
    }
    return OK;
}