#ifndef __CAN__
#define __CAN__

#include "driver/twai.h"
#include <stdio.h>
#include "esp_log.h"
#include <map>
#include "freertos/semphr.h"
#include <tuple>

namespace CAN{
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
        private:
            uint64_t last_recieved;
        
    };
    template<class Type>
    class Signal{
        public:
            typedef enum{
                OK,
                PARSE_ERROR,
                SERIALIZATION_ERROR,    
            }SIGNAL_ERROR;
            virtual T get(); 
            virtual SIGNAL_ERROR set(T value); 
             
        private:
            T value;
            int scale;
            int offset;
    };

}// CAN namespace





#endif