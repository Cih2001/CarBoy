#ifndef GLOBAL_H
#define GLOBAL_H
#include <log_ctrl.h>

#ifdef MovementCtrl
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
