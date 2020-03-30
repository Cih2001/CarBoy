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

    // Now we need to process the position
    JoystickCommandEvent commandEvent;
    memcpy(commandEvent.AxesStatus, _axes, sizeof(_axes));
    memcpy(commandEvent.ButtonStatus, _buttons, sizeof(_buttons));
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
