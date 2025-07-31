#include "WFR_Lib/CAN.hpp"
#include <stdio.h>

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