#ifndef LOG_CONTROLLER
#define LOG_CONTROLLER

#include <curses.h>

class LogContrller {
public:
    LogContrller();
    ~LogContrller();

    int printf(const char *format, ...);
};
#endif
