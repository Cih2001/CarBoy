#ifndef CARBOY_GLOBAL_H_
#include "log/log_ctrl.h"

#ifdef CARBOY_MOVEMENT_MOVEMENT_CTRL_H_
    #define EXTERN
#else
    #define EXTERN extern
#endif


EXTERN LogContrller logController;

template<typename T> T map(T val, T ir1, T ir2, T fr1, T fr2)
{
    float pos = (float)(val - ir1) / (float)( ir2 - ir1);
    auto result = (float)(fr2 - fr1) * pos + (float)fr1;
    return (T) result;
}


#endif
