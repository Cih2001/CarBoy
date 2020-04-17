#include "log/log_ctrl.h"

#include "global.h"

#include <iostream>
#include <algorithm>

Object::Object(std::shared_ptr<Window> parent, 
    std::string name,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height)
{
    name_ = name;
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
    parent_ = parent;
}

void Object::clear() { 
    for (unsigned int i = 0; i < height_; i++)
        mvwhline(parent_->wnd_, y_ + i, x_ , ' ', width_);
}

std::string Object::getName() {
    return name_;
}

/////////////////////////////////////////////////////////
// Frame implementations
/////////////////////////////////////////////////////////

Frame::Frame(std::shared_ptr<Window> parent,
    std::string name,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height,
    std::string title) : Object(parent, name, x, y, width, height)
{
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
    this->clear();
    wattron(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
    rectangle(parent_->wnd_, y_, x_, y_ + height_ - 1, x_ + width_ - 1);	
   
    if (title_.empty()) {
        wattroff(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
        return;
    }
    // Draw Title
    std::string title = " " + title_ + " ";
    int start = 0; 
    switch (title_alignment_) {
    case ALIGN_CENTER:
        start = x_ + (width_ - title.length() - 2) / 2;
        break;
    case ALIGN_LEFT:
        start = x_ + 2;
        break;
    case ALIGN_RIGHT:
        start = x_ + (width_ - title.length() - 4);
        break;
    }
    parent_->move(start , y_);
    waddch(parent_->wnd_, ACS_RTEE);
    wattroff(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
    wprintw(parent_->wnd_, title.c_str());
    wattron(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
    waddch(parent_->wnd_, ACS_LTEE);
    wattroff(parent_->wnd_, COLOR_PAIR(BORDERS_COLOR));
}

void Frame::setAlignment(Alignment alignment) {
    title_alignment_ = alignment;
}

void Frame::setTitle(std::string title) {
    title_ = title;
}

/////////////////////////////////////////////////////////
// AutoScrollLabel implementations
/////////////////////////////////////////////////////////

AutoScrollLabel::AutoScrollLabel(
    std::shared_ptr<Window> parent,
    std::string name,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height
) : Object(parent, name, x, y, width, height) {

}

void AutoScrollLabel::redraw() {
    int count = lines_.size();
    auto blank = std::string(width_, ' ');
    this->clear();

    for ( int i = 0; i < count; i++) {
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

Window::~Window() {
    delwin(wnd_);
}

void Window::refreshWindow() {
    for (auto child : children_) {
        child->redraw();
    }
    wrefresh(wnd_);
}

void Window::move(unsigned int x, unsigned int y) {
    cursor_x_ = x;
    cursor_y_ = y;
    wmove(wnd_, y, x);
}

bool Window::addChild(std::shared_ptr<Object> obj) {
    // we have to check if the object does not exists
    children_.push_back(obj); 
    return true;
}

std::shared_ptr<Object> Window::getObjectByName(std::string name) {
    for (auto child : children_) {
        if (child->getName() == name)
            return child;
    }
    return nullptr;
}

/////////////////////////////////////////////////////////
// HorizentalGauge implementations
/////////////////////////////////////////////////////////

template <typename T>
HorizentalGauge<T>::HorizentalGauge(
    std::shared_ptr<Window> parent,
    std::string name,
    unsigned int x,
    unsigned int y,
    unsigned int width) : Object(parent, name, x, y, width, 1)
{ 
    min_ = max_ = value_ = 0;
}

template <typename T>
void HorizentalGauge<T>::redraw() {
    this->clear();

    auto mapped_val = map(value_, min_, max_, - (T)width_ / 2, (T)width_ / 2);
    if (mapped_val > 0) {
        mvwhline(parent_->wnd_, y_, x_ + width_ / 2, '|', mapped_val);
    } else {
        mvwhline(parent_->wnd_, y_, x_ + width_ / 2 + mapped_val, '|', -mapped_val);
    }
    mvwaddch(parent_->wnd_, y_, x_ + width_ / 2, ACS_CKBOARD);
}

template <typename T>
void HorizentalGauge<T>::setValue(T value) {
    min_ = std::min(min_, value); 
    max_ = std::max(max_, value); 
    if (max_ < -min_) {
        max_ = -min_; 
    }
    else {
        min_ = -max_;
    }
    value_ = value;
}

/////////////////////////////////////////////////////////
// Label implementations
/////////////////////////////////////////////////////////

Label::Label(
    std::shared_ptr<Window> parent,
    std::string name,
    unsigned int x,
    unsigned int y,
    unsigned int width,
    std::string caption
) : Object(parent, name, x, y, width, 1) 
{
    caption_ = caption;
}

void Label::setCaption(const std::string caption) {
    caption_ = caption;
}

void Label::redraw() {
    this->clear();
    unsigned int x;
    switch (alignment_) {
        case ALIGN_LEFT:
            x = x_;
            break;
        case ALIGN_CENTER:
            x = x_ + (width_ - caption_.size()) / 2;
            break;
        case ALIGN_RIGHT:
            x = x_ + width_ - caption_.size();
            break;
    }
    mvwprintw(parent_->wnd_, y_, x, "%s", caption_.c_str());
    // Clean gauge
}

void Label::setAlignment(Alignment alignment) {
    alignment_ = alignment;
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
        "fraLogs",
        0 , 0, screen_width_ / 2, screen_height_,
        "Logs"
    );
    auto auto_scroll_label = std::make_shared<AutoScrollLabel> (
        right_window_,
        "aslLogs",
        1, 1, screen_width_ / 2 - 2, screen_height_ - 2
    );
    right_window_->addChild(frame);
    right_window_->addChild(auto_scroll_label);
    right_window_->refreshWindow();

    left_window_ = std::make_shared<Window>(
        0, 0, screen_width_ / 2, screen_height_
    );
    auto left_frame = std::make_shared<Frame> (
        left_window_,
        "frmStats",
        0 , 0, screen_width_ / 2, screen_height_,
        "Stats"
    );
    left_window_->addChild(left_frame);
    for (unsigned int i = 0; i < 8; i++) {
        std::string gauge_title;
        switch (i % 4) {
        case 0:
            gauge_title = "Front Left Motor";
            break;
        case 1:
            gauge_title = "Front Right Motor";
            break;
        case 2:
            gauge_title = "Rear Left Motor";
            break;
        case 3:
            gauge_title = "Rear Right  Motor";
            break;
        }
        auto gauge_border = std::make_shared<Frame> (
            left_window_,
            "fraGagueBorder" + std::to_string(i),
            2 , 2 + i * 4, screen_width_ / 2 - 4, 3,
            gauge_title
        );
        gauge_border->setAlignment(ALIGN_LEFT);
        left_window_->addChild(gauge_border);
        if ( i < 4 ) {
            auto gauge = std::make_shared<HorizentalGauge<int>> (
                left_window_,
                "gau" + std::to_string(i),
                3 , 3 + i * 4, screen_width_ / 2 - 6
            );
            left_window_->addChild(gauge);
        } else {
            auto gauge = std::make_shared<HorizentalGauge<float>> (
                left_window_,
                "gau" + std::to_string(i),
                3 , 3 + i * 4, screen_width_ / 2 - 6
            );
            left_window_->addChild(gauge);
        }
    }

    auto lbl1 = std::make_shared<Label> (
        left_window_,
        "lbl1",
        3 , 40, screen_width_ / 2 - 6,
        "Label 1"
    );
    left_window_->addChild(lbl1);
    auto lbl2 = std::make_shared<Label> (
        left_window_,
        "lbl2",
        3 , 41, screen_width_ / 2 - 6,
        "Label 2"
    );
    left_window_->addChild(lbl2);
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
    auto label = std::dynamic_pointer_cast<AutoScrollLabel>(
        right_window_->getObjectByName("aslLogs")
    );
    if (label != nullptr)
        label->println(str);
    right_window_->refreshWindow();
    return res;
}

void LogContrller::updateMotorSpeed(unsigned int idx, int speed) {
    auto gauge = std::dynamic_pointer_cast<HorizentalGauge<int>>(
        left_window_->getObjectByName("gau" + std::to_string(idx))
    );
    if (gauge != nullptr)
    {
        gauge->setValue(speed);
    }

    auto frame = std::dynamic_pointer_cast<Frame>(
        left_window_->getObjectByName("fraGagueBorder" + std::to_string(idx))
    );
    if (frame != nullptr)
    {
        std::string gauge_title;
        switch (idx) {
        case 0:
            gauge_title = "Front Left Motor: ";
            break;
        case 1:
            gauge_title = "Front Right Motor: ";
            break;
        case 2:
            gauge_title = "Rear Left Motor: ";
            break;
        case 3:
            gauge_title = "Rear Right  Motor: ";
            break;
        }
        gauge_title += std::to_string(speed);
        frame->setTitle(gauge_title);
    }
    left_window_->refreshWindow();
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
    curs_set(0);
    intrflush(stdscr, false);
    keypad(stdscr, true);

    // Initializing color pairs.
    init_pair(BORDERS_COLOR, COLOR_RED, COLOR_BLACK);

    isNcursesInitialized = true;
}

void LogContrller::updateEncoderSpeed(int val1, float val2) {
    auto gauge = std::dynamic_pointer_cast<HorizentalGauge<float>>(
        left_window_->getObjectByName("gau4")
    );
    if (gauge != nullptr)
    {
        gauge->setValue(val2);
    }

    auto lbl1 = std::dynamic_pointer_cast<Label>(left_window_->getObjectByName("lbl1"));
    auto lbl2 = std::dynamic_pointer_cast<Label>(left_window_->getObjectByName("lbl2"));
    lbl1->setCaption(std::to_string(val1));
    lbl2->setCaption(std::to_string(val2));
    left_window_->refreshWindow();
}
