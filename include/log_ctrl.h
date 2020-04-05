#ifndef LOG_CONTROLLER
#define LOG_CONTROLLER

#include <curses.h>
#include <memory>
#include <string>
#include <vector>
#include <deque>

enum ColorPairs {
    BORDERS_COLOR = 1
};

class Window;

class Object {
public:
    Object(std::shared_ptr<Window> parent,
        unsigned int x,
        unsigned int y,
        unsigned int width,
        unsigned int height
    );
    virtual void redraw() = 0;
protected:
    std::shared_ptr<Window> parent_;
    unsigned int x_, y_, width_, height_;
};


class Window {
public:
    explicit Window(
        unsigned int x,
        unsigned int y,
        unsigned int width,
        unsigned int height
    );
    ~Window();
    WINDOW* wnd_ = nullptr;

    // Moves cursor to a position on window.
    void move(unsigned int x, unsigned int y);

    // Margin move, move cursor to a position on window considering margines as well.
    void mmove(unsigned int x, unsigned int y);

    // Refreshes the window.
    void refreshWindow();

    void setMargin(unsigned int margin = 0);
    unsigned int getX0();
    unsigned int getY0();
    unsigned int getX1();
    unsigned int getY1();

    void addObject(std::shared_ptr<Object>);

    // TODO: should be moved to private section and whole structure should be changed.
    std::vector<std::shared_ptr<Object>> children_;

protected:
    unsigned int x_, y_, width_, height_;
    unsigned int cursor_x_, cursor_y_;

    // Margin is used to restrict printing area, padding from borders.
    unsigned int margin_ = 0;

    unsigned int getCursorX();   // returns relative cursor x from the window x
    unsigned int getCursorY();   // returns relative cursor y from the window y
    unsigned int getmCursorX();  // returns relative cursor x from the margin x
    unsigned int getmCursorY();  // returns relative cursor y from the margin y

};

class Frame : public Object {
public:
    explicit Frame(std::shared_ptr<Window> parent,
        unsigned int x,
        unsigned int y,
        unsigned int width,
        unsigned int height,
        std::string title
    );

    void redraw() override;

private:
    std::string title_;
};

class AutoScrollLabel : public Object {
public:
    explicit AutoScrollLabel(std::shared_ptr<Window> parent,
        unsigned int x,
        unsigned int y,
        unsigned int width,
        unsigned int height
    );

    void redraw() override;

    void println(std::string line);

private:
    std::deque<std::string> lines_;
};

class LogContrller {
public:
    // side note: technically everything in LogContrller should be static. We will
    // only have one instance for LogContrller. No one should be able to create two
    // or more instances of LogContrller.
    // This also can be converted into a singleton class but as I hate singletons,
    // I will leave it as it is.
    static bool isNcursesInitialized;
    static void initializeNcurses();

    LogContrller();
    ~LogContrller();

    int printf(const char *format, ...);

private:
    unsigned int screen_width_;
    unsigned int screen_height_;

    // Log controller has two windows side by side, left side is to show statistics.
    // right window is to show print logs.
    std::shared_ptr<Window> left_window_;
    std::shared_ptr<Window> right_window_;
};

#endif
