#ifndef CARBOY_LOG_LOG_CTRL_H_
#define CARBOY_LOG_LOG_CTRL_H_

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
        std::string name,
        unsigned int x,
        unsigned int y,
        unsigned int width,
        unsigned int height
    );
    
    virtual void redraw() = 0;
    virtual void clear();

    std::string getName();
protected:
    std::shared_ptr<Window> parent_;
    unsigned int x_, y_, width_, height_;
    std::string name_;
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

    // Refreshes the window.
    void refreshWindow();

    bool addChild(std::shared_ptr<Object>);

    std::shared_ptr<Object> getObjectByName(std::string);

    // TODO: should be moved to private section and whole structure should be changed.
    std::vector<std::shared_ptr<Object>> children_;

protected:
    unsigned int x_, y_, width_, height_;

    // TODO: so far in the current app we don't need these. even in some places,
    // we change the window cursor position without updating them.
    // However, I keep them for now, maybe in the future we need them.
    unsigned int cursor_x_, cursor_y_;

    // Margin is used to restrict printing area, padding from borders.
    unsigned int margin_ = 0;

    unsigned int getCursorX();   // returns relative cursor x from the window x
    unsigned int getCursorY();   // returns relative cursor y from the window y
    unsigned int getmCursorX();  // returns relative cursor x from the margin x
    unsigned int getmCursorY();  // returns relative cursor y from the margin y

};

// Used in different objects for aligning titles or captions.
enum Alignment {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
};

class Frame : public Object {
public:
    explicit Frame(std::shared_ptr<Window> parent,
        std::string name,
        unsigned int x,
        unsigned int y,
        unsigned int width,
        unsigned int height,
        std::string title
    );

    void redraw() override;
    void setAlignment(Alignment);
    void setTitle(std::string);

private:
    std::string title_;
    Alignment title_alignment_ = ALIGN_CENTER;
};

class AutoScrollLabel : public Object {
public:
    explicit AutoScrollLabel(std::shared_ptr<Window> parent,
        std::string name,
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

template <typename T>
class HorizentalGauge : public Object {
public:
    explicit HorizentalGauge(
        std::shared_ptr<Window> parent,
        std::string name,
        unsigned int x,
        unsigned int y,
        unsigned int width
    );
    
    void redraw() override;
    void setValue(T);
    T getValue();

private:
    T min_, max_, value_;
};

class Label: public Object {
public:
    explicit Label(
        std::shared_ptr<Window> parent,
        std::string name,
        unsigned int x,
        unsigned int y,
        unsigned int width,
        std::string caption
    );

    void redraw() override;
    void setCaption(const std::string);
    void setAlignment(Alignment);

protected:
    std::string caption_;
    Alignment alignment_ = ALIGN_LEFT;
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
    void updateMotorSpeed(unsigned int idx, int speed);
    void updateEncoderSpeed(int val1, float val2);

private:
    unsigned int screen_width_;
    unsigned int screen_height_;

    // Log controller has two windows side by side, left side is to show statistics.
    // right window is to show print logs.
    std::shared_ptr<Window> left_window_;
    std::shared_ptr<Window> right_window_;
};

#endif
