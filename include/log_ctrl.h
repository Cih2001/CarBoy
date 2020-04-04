#ifndef LOG_CONTROLLER
#define LOG_CONTROLLER

#include <curses.h>
#include <memory>
#include <string>

class Window {
public:
    explicit Window(int x, int y, int width, int height);
    ~Window();
    WINDOW* wnd_;

    // Moves cursor to a position on window.
    void move(int x, int y);

    // Margin move, move cursor to a position on window considering margines as well.
    void mmove(int x, int y);

    void println(std::string line);

    // Refreshes the window.
    void refresh();

    void setMargin(int margin = 0);
    int getX0();
    int getY0();
    int getX1();
    int getY1();

protected:
    int x_, y_, width_, height_;
    int cursor_x_, cursor_y_;

    // Margin is used to restrict printing area, padding from borders.
    int margin_ = 0;
};

class FramedWindow : public Window {
public:
    explicit FramedWindow(int x, int y, int width, int height
        , std::string title
    );

private:
    std::string title_;
    void drawBorders();
};

class LogContrller {
public:
    LogContrller();
    ~LogContrller();

    int printf(const char *format, ...);

private:
    int screen_width_;
    int screen_height_;

    // Log controller has two windows side by side, left side is to show statistics.
    // right window is to show print logs.
    std::unique_ptr<Window> left_window_;
    std::unique_ptr<Window> right_window_;
};
#endif
