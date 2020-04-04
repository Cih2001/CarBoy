#include "log_ctrl.h"
#include <iostream>

LogContrller::LogContrller() {
    initscr();
    nonl();
    intrflush(stdscr, false);
    keypad(stdscr, true);
}

LogContrller::~LogContrller() {
    endwin();
}

int LogContrller::printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int res = vprintf(format, args);
    va_end(args);
    return res;
}


