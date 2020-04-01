#include "JoystickCtrl.h"
#include <unistd.h>
#include <string.h>


int JoystickController::Start() {
    // Initializing joystick
    Joystick joystick(_path.c_str());
    if (!joystick.isFound()) {
        return -1;
    }   
   
    
    JoystickEvent event;
    for (;;) {
        usleep(1000);
        if (joystick.sample(&event)) {
            handleNewEvent(event);
        }
    }
    return 0;
}

template<typename T> T map(T val, T ir1, T ir2, T fr1, T fr2)
{
    T pos = (val - ir1) / ( ir2 - ir1);
    return (fr2 - fr1) * pos + fr1;
}

void JoystickController::handleNewEvent(JoystickEvent& event) {
    if (event.isButton()) {
        if (event.value == 0) {
            // Button up.
            _buttons[event.number] = false;
        }
        else {
            // Button down
            _buttons[event.number] = true;
        }
    } else if (event.isAxis()) {
        _axes[event.number] = event.value;
    }

    JoystickCommandEvent commandEvent;
    memcpy(commandEvent.AxesStatus, _axes, sizeof(_axes));
    memcpy(commandEvent.ButtonStatus, _buttons, sizeof(_buttons));

    // computing relative speed for all motors.
    if (event.number == AXIS_L2) {
        // just reducing speed
        commandEvent.CommandCode = CMD_SET_SPEED;
        int percent = -map((int)_axes[AXIS_L2], -32768, 32767, 0, 100);
        commandEvent.GlobalSpeedInPercent = percent;
        sendCommand(commandEvent);

        return;
    }
    if (event.number == AXIS_R2) {
    }

    // Now we need to process the position
    if (_buttons[BTN_UP]) {
        commandEvent.CommandCode = CMD_MOVE_FORWARD;
        sendCommand(commandEvent);
        return;
    }
    if (_buttons[BTN_DOWN]) {
        commandEvent.CommandCode = CMD_MOVE_BACKWARD;
        sendCommand(commandEvent);
        return;
    }
    // Non of the above commands. so we sent stop command.
    commandEvent.CommandCode = CMD_STOP;
    write(_pipe_fd, &commandEvent, sizeof(commandEvent));
    return;
}

void JoystickController::sendCommand(JoystickCommandEvent& event) {
    write(_pipe_fd, &event, sizeof(event));
    // TODO: check if error
    return;
}
