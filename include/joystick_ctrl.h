#ifndef JoystickCntrl
#define JoystickCntrl

#include <memory>
#include <string>
#include "joystick.h"

#define BTN_CROSS 0
#define BTN_CIRCLE 1
#define BTN_TRIANGLE 2
#define BTN_SQUARE 3
#define BTN_L1 4
#define BTN_R1 5
#define BTN_L2 6
#define BTN_R2 7
#define BTN_SELECT 8
#define BTN_START 9
#define BTN_HOME 10
#define BTN_THUMBL 11
#define BTN_THUMBR 12
#define BTN_UP 13
#define BTN_DOWN 14
#define BTN_LEFT 15
#define BTN_RIGHT 16
#define NUM_OF_BUTTONS 17

#define AXIS_L1_HORIZENTAL 0
#define AXIS_L1_VERTICAL 1
#define AXIS_R1_HORIZENTAL 3
#define AXIS_R1_VERTICAL 4
#define AXIS_L2 2
#define AXIS_R2 5
#define NUM_OF_AXES 6


enum Commands {
    CMD_STOP,
    CMD_SET_SPEED,
    CMD_MOVE_FORWARD,
    CMD_MOVE_BACKWARD
};

struct __attribute__ ((packed)) JoystickCommandEvent {
    Commands CommandCode;
    int GlobalSpeedInPercent;
    bool ButtonStatus[NUM_OF_BUTTONS];
    short AxesStatus[NUM_OF_AXES];
};

typedef void (JoystickEventHandler)(std::shared_ptr<JoystickCommandEvent>);

class JoystickController {
public:
    JoystickController(const char* path, int pipe_fd) :
        _path(std::string(path)), _pipe_fd(pipe_fd){};
    int Start();
private:
    std::string _path;
    int _pipe_fd;

    // These are the states of the joystick
    bool _buttons[NUM_OF_BUTTONS];
    short _axes[NUM_OF_AXES];

    void handleNewEvent(JoystickEvent& event);
    void sendCommand(JoystickCommandEvent& event);
};
#endif
