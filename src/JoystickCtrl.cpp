#include "JoystickCtrl.h"

JoystickController::JoystickController(
    const char* path,
    JoystickEventHandler* fnEventsCallback
) {
    if (fnEventsCallback != nullptr) {
        _fnEventHandler = fnEventsCallback;
    }

}
