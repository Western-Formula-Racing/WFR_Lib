#ifndef __CAN__
#define __CAN__

#include "driver/twai.h"
#include <stdio.h>
#include "esp_log.h"
#include <map>
#include "freertos/semphr.h"
#include <tuple>
#define TIMEOUT 2000
namespace CAN{

    typedef enum{
                OK,
                ERROR,    
    }CAN_ERROR;

    typedef enum{
                OK,
                PARSE_ERROR,
                SERIALIZATION_ERROR,    
    }SIGNAL_ERROR;
    struct CanFrame
    {
        uint32_t id;
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
            virtual MESSAGE_ERROR parse(uint64_t* buffer);
            virtual MESSAGE_ERROR serialize(uint64_t* buffer);
            uint32_t id;
        protected:
            uint64_t last_recieved;
        
    };
    template<class Type>
    class Signal{
        public:
        
            Type get(); 
            SIGNAL_ERROR set(Type value); 
            Signal(float scale, float offset, uint64_t* last_recieved_p, Type default_value = 0); 
        private:
            Type value;
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
            uint16_t txCallBackCounter; 
            static void rx_task_wrapper(void *arg); // Wrapper function
            static void tx_CallBack_wrapper(TimerHandle_t xTimer); // Wrapper function
            void tx_CallBack(); // Non-static member function
            void rx_task(); // Non-static member function
            virtual CAN_ERROR rx_msg(CAN::CanFrame* can_frame); //each interface needs it's own implementation
            virtual CAN_ERROR tx_msg(CAN::CanFrame* can_frame); //each interface needs it's own implementation 
            static TaskHandle_t rxTaskHandle;
            static TimerHandle_t timerHandle;
            static SemaphoreHandle_t rx_sem;
    };


}// CAN namespace
#endif