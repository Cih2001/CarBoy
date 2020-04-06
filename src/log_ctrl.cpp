#include "log_ctrl.h"
#include <iostream>

Object::Object(std::shared_ptr<Window> parent, 
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height) {
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
    parent_ = parent;
}

/////////////////////////////////////////////////////////
// Frame implementations
/////////////////////////////////////////////////////////

Frame::Frame(std::shared_ptr<Window> parent,
        unsigned int x,
        unsigned int y,
        unsigned int width,
        unsigned int height,
        std::string title) :
    Object(parent, x, y, width, height) {
    title_ = title;
}

void rectangle(WINDOW* wnd, int y1, int x1, int y2, int x2)
{
    mvwhline(wnd, y1, x1, 0, x2-x1);
    mvwhline(wnd, y2, x1, 0, x2-x1);
    mvwvline(wnd, y1, x1, 0, y2-y1);
    mvwvline(wnd, y1, x2, 0, y2-y1);
    mvwaddch(wnd, y1, x1, ACS_ULCORNER);
    mvwaddch(wnd, y2, x1, ACS_LLCORNER);
    mvwaddch(wnd, y1, x2, ACS_URCORNER);
    mvwaddch(wnd, y2, x2, ACS_LRCORNER);
}

void Frame::redraw() {
    wattron(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
    rectangle(parent_->wnd_, y_, x_, y_ + height_ - 1, x_ + width_ - 1);	
    
    // Draw Title
    std::string title = " " + title_ + " ";
    int start = (width_ - title.length() - 2) / 2;
    parent_->move(start , 0);
    waddch(parent_->wnd_, ACS_RTEE);
    wattroff(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
    wprintw(parent_->wnd_, title.c_str());
    wattron(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
    waddch(parent_->wnd_, ACS_LTEE);
    wattroff(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
}

/////////////////////////////////////////////////////////
// AutoScrollLabel implementations
/////////////////////////////////////////////////////////

AutoScrollLabel::AutoScrollLabel(
    std::shared_ptr<Window> parent,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
) : Object(parent, x, y, width, height) {

}

void AutoScrollLabel::redraw() {
    int count = lines_.size();
    auto blank = std::string(width_, ' ');

    for ( int i = 0; i < count; i++) {
        // First we need to clear the row.
        mvwhline(parent_->wnd_, y_ + i, x_, ' ', width_);

        // Now we write the text
        parent_->move(x_, y_ + i);
        wprintw(parent_->wnd_, lines_[i].c_str());
    }
}

void AutoScrollLabel::println(std::string line) {
    line.erase(line.find_last_not_of(" \n\r\t") + 1);
    lines_.push_back(line);
    while (lines_.size() > height_)
        lines_.pop_front();
}

/////////////////////////////////////////////////////////
// Window implementations
/////////////////////////////////////////////////////////

Window::Window(
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
) {
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
    
    // Initializing window.
    wnd_ = newwin(height, width, y, x);
}

void Window::setMargin(unsigned int margin) {
    margin_ = margin;
}

Window::~Window() {
    delwin(wnd_);
}

void Window::refreshWindow() {
    for (auto child : children_) {
        child.second->redraw();
    }
    wrefresh(wnd_);
}

unsigned int Window::getX0() {
    return x_;
}

unsigned int Window::getX1() {
    return x_ + width_;
}

unsigned int Window::getY0() {
    return y_;
}

unsigned int Window::getY1() {
    return y_ + height_;
}

void Window::move(unsigned int x, unsigned int y) {
    cursor_x_ = x;
    cursor_y_ = y;
    wmove(wnd_, y, x);
}

void Window::mmove(unsigned int x, unsigned int y) {
    move(x + margin_, y + margin_);
}

unsigned int Window::getCursorX() {
    return cursor_x_;
}

unsigned int Window::getCursorY() {
    return cursor_y_;
}

unsigned int Window::getmCursorX() {
    return cursor_x_ - margin_;
}

unsigned int Window::getmCursorY() {
    return cursor_y_ - margin_;
}

void Window::addElement(Element elm) {
    children_.push_back(elm); 
}

std::shared_ptr<Object> Window::getObjectByName(std::string name) {
    for (auto child : children_) {
        if (child.first == name)
            return child.second;
    }
    return nullptr;
}

/////////////////////////////////////////////////////////
// LogContrller implementations
/////////////////////////////////////////////////////////

LogContrller::LogContrller() {
    // Initializing ncurses
    LogContrller::initializeNcurses();

    // Getting screen size.
    getmaxyx(stdscr, screen_height_, screen_width_);

    // Create log window.
    right_window_ = std::make_shared<Window>(
        screen_width_ / 2, 0, screen_width_ / 2, screen_height_
    );
    auto frame = std::make_shared<Frame> (
        right_window_,
        0 , 0, screen_width_ / 2, screen_height_,
        "Logs"
    );
    auto auto_scroll_label = std::make_shared<AutoScrollLabel> (
        right_window_,
        1, 1, screen_width_ / 2 - 2, screen_height_ - 2
    );
    right_window_->addElement(Element("frmMain", frame));
    right_window_->addElement(Element("auto1", auto_scroll_label));
    right_window_->refreshWindow();

    left_window_ = std::make_shared<Window>(
        0, 0, screen_width_ / 2, screen_height_
    );
    auto left_frame = std::make_shared<Frame> (
        left_window_,
        0 , 0, screen_width_ / 2, screen_height_,
        "Stats"
    );
    left_window_->addElement(Element("frmMain", left_frame));
    left_window_->refreshWindow();
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
    auto label = std::static_pointer_cast<AutoScrollLabel>(
        right_window_->getObjectByName("auto1")
    );
    if (label != nullptr)
        label->println(str);
    right_window_->refreshWindow();
    return res;
}

// Initializing static boolean variable.
bool LogContrller::isNcursesInitialized = false;

// An static function to initialize ncurses only once.
void LogContrller::initializeNcurses() {
    if ( isNcursesInitialized ) return;

    setlocale(LC_ALL, "");
    initscr();
    start_color();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, false);
    keypad(stdscr, true);

    // Initializing color pairs.
    init_pair(BORDERS_COLOR, COLOR_RED, COLOR_BLACK);

    isNcursesInitialized = true;
}
