#ifndef GLOBAL_H
#define GLOBAL_H
#include <log_ctrl.h>

#ifdef MovementCtrl
  #define EXTERN
#else
  #define EXTERN extern
#endif

EXTERN LogContrller logController;

#endif
