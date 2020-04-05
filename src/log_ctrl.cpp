#include "log_ctrl.h"
#include <iostream>


FramedWindow::FramedWindow(int x, int y, int width, int height,
        std::string title) :
    Window(x, y, width, height) {
    init_pair(1, COLOR_RED, COLOR_BLACK);
    
    title_ = title;
    drawBorders();
    setMargin(1);
    mmove(0, 0);
    refreshWindow();
}

void FramedWindow::drawBorders() {
    wattron(wnd_, COLOR_PAIR(1));
    wborder(wnd_, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    // Draw Title
    std::string title = " " + title_ + " ";
    int start = (width_ - title.length() - 2) / 2;
    move(start , 0);
    waddch(wnd_, ACS_RTEE);
    wattroff(wnd_, COLOR_PAIR(1));
    wprintw(wnd_, title.c_str());
    wattron(wnd_, COLOR_PAIR(1));
    waddch(wnd_, ACS_LTEE);
    wattroff(wnd_, COLOR_PAIR(1));
    refreshWindow();
}


Window::Window(int x, int y, int width, int height) {
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
    
    // Initializing window.
    wnd_ = newwin(height, width, y, x);
}

void Window::setMargin(int margin) {
    margin_ = margin;
}

Window::~Window() {
    delwin(wnd_);
}

void Window::refreshWindow() {
    wrefresh(wnd_);
    // refresh();
}

int Window::getX0() {
    return x_;
}

int Window::getX1() {
    return x_ + width_;
}

int Window::getY0() {
    return y_;
}

int Window::getY1() {
    return y_ + height_;
}

void Window::move(int x, int y) {
    cursor_x_ = x;
    cursor_y_ = y;
    wmove(wnd_, y, x);
}

void Window::mmove(int x, int y) {
    move(x + margin_, y + margin_);
}

void Window::println(std::string line) {
    line.erase(line.find_last_not_of(" \n\r\t") + 1);
    wprintw(wnd_, "%s", line.c_str());
    mmove(0, getmCursorY() + 1);
    refreshWindow();
}

int Window::getCursorX() {
    return cursor_x_;
}

int Window::getCursorY() {
    return cursor_y_;
}

int Window::getmCursorX() {
    return cursor_x_ - margin_;
}

int Window::getmCursorY() {
    return cursor_y_ - margin_;
}


/////////////////////////////////////////////////////////
// LogContrller implementations
/////////////////////////////////////////////////////////

LogContrller::LogContrller() {
    // Initializing ncurses
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    // cbreak();
    // noecho();
    // nonl();
    // intrflush(stdscr, false);
    // keypad(stdscr, true);
    // refresh();

    // Getting screen size.
    getmaxyx(stdscr, screen_height_, screen_width_);

    // Create log window.
    right_window_ = std::make_unique<FramedWindow>(
        screen_width_/2,0,screen_width_/2, screen_height_, "Logs"
    );

    left_window_ = std::make_unique<FramedWindow>(
        0,0,screen_width_/2, screen_height_, "Stats"
    );
}

LogContrller::~LogContrller() {
    endwin();
}

int LogContrller::printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buff[265];
    int res = vsprintf(buff, format, args);
    va_end(args);
    std::string str(buff);

    right_window_->println(str);
    return res;
}
