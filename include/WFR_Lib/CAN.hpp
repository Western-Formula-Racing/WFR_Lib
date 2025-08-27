#ifndef __CAN__
#define __CAN__

#include "driver/twai.h"
#include <stdio.h>
#include "esp_log.h"
#include "esp_timer.h"
#include <map>
#include "freertos/semphr.h"
#include <tuple>
#include <string.h>
#define TIMEOUT 2000
namespace CAN{

    typedef enum{
                OK,
                RX_QUEUE_EMPTY,
                TX_ERROR,
                ERROR,
                TWAI_DRIVER_ERROR,    
    }CAN_ERROR;

    typedef enum{
                SIGNAL_OK,
                PARSE_ERROR,
                SERIALIZATION_ERROR,    
    }SIGNAL_ERROR;
    struct CanFrame
    {
        uint32_t id;
        uint8_t dlc;
        uint32_t extd;
        uint8_t buffer[8];
    };

    class Message{
        public:
            typedef enum{
                OK,
                PARSE_ERROR,
                SERIALIZATION_ERROR,    
            }MESSAGE_ERROR;
            bool timed_out();
            virtual MESSAGE_ERROR parse(uint8_t buffer[]) = 0;
            virtual MESSAGE_ERROR serialize(uint8_t buffer[]) = 0;
            uint32_t id;
        protected:
            uint64_t last_recieved;
        
    };
    template<class Type>
    class Signal{
        public:
        
            Type get(){
                int64_t current_time = esp_timer_get_time()/1000;
                if((current_time - *last_recieved_p) >= TIMEOUT){
                    return default_value;
                }
                return value;
            }; 
            SIGNAL_ERROR set(Type raw_value){
                value = static_cast<Type>((raw_value*scale)+offset);
                return SIGNAL_ERROR::SIGNAL_OK;
            }; 
            Signal(uint8_t start_bit, uint8_t bit_length, float scale, float offset, uint64_t* last_recieved_p, Type default_value = 0): 
            start_bit(start_bit), bit_length(bit_length), scale(scale), offset(offset), last_recieved_p(last_recieved_p), default_value(default_value)
            {
            }
        private:
            Type value;
            uint8_t start_bit;
            uint8_t bit_length;
            float scale;
            float offset;
            uint64_t* last_recieved_p;
            Type default_value;
    };




    class BaseInterface
    {
        public:
            
            CAN_ERROR begin();
            bool logging;
            int restart_counter;
        protected:
            virtual CAN_ERROR init() = 0;
            uint16_t txCallBackCounter; 
            static void rx_task_wrapper(void *arg); // Wrapper function
            static void tx_CallBack_wrapper(TimerHandle_t xTimer); // Wrapper function
            void tx_CallBack(); // Non-static member function
            void rx_task(); // Non-static member function
            virtual CAN_ERROR rx_msg(CAN::CanFrame* can_frame) = 0; //each interface needs it's own implementation
            virtual CAN_ERROR tx_msg(CAN::CanFrame* can_frame) = 0; //each interface needs it's own implementation 
            TaskHandle_t rxTaskHandle;
            TimerHandle_t timerHandle;
            SemaphoreHandle_t rx_sem;
    };

    class TWAI_Interface: public BaseInterface
    {
        public:
            TWAI_Interface(gpio_num_t CAN_Tx_Pin, gpio_num_t CAN_Rx_Pin, twai_mode_t twai_mode);
        protected:
            CAN_ERROR init();
            CAN_ERROR rx_msg(CAN::CanFrame* can_frame) override;
            CAN_ERROR tx_msg(CAN::CanFrame* can_frame) override;
            const char* get_twai_error_state_text(twai_status_info_t* status);
        private:
            twai_handle_t twai_handle;
            gpio_num_t CAN_Tx_Pin;
            gpio_num_t CAN_Rx_Pin;
            twai_mode_t twai_mode;
    };

}
// CAN namespace
#endif