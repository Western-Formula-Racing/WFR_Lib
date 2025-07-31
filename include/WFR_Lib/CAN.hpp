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
                PARSE_ERROR,
                SERIALIZATION_ERROR,    
    }SIGNAL_ERROR;
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

}// CAN namespace





#endif