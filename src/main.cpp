#include <iostream>
#include <signal.h>
#include "joystick.h"
#include "MovementCtrl.h"
#include <wiringPi.h>

#define FRONT_LEFT_MOTOR_PWM 0
#define FRONT_LEFT_MOTOR_EN1 1
#define FRONT_LEFT_MOTOR_EN2 2

#define FRONT_RIGHT_MOTOR_PWM 13
#define FRONT_RIGHT_MOTOR_EN1 11
#define FRONT_RIGHT_MOTOR_EN2 12

#define REAR_LEFT_MOTOR_PWM 5
#define REAR_LEFT_MOTOR_EN1 3
#define REAR_LEFT_MOTOR_EN2 4

#define REAR_RIGHT_MOTOR_PWM 8
#define REAR_RIGHT_MOTOR_EN1 9
#define REAR_RIGHT_MOTOR_EN2 10

MovementController movementCtrl(
    FRONT_LEFT_MOTOR_PWM, FRONT_LEFT_MOTOR_EN1, FRONT_LEFT_MOTOR_EN2,
    FRONT_RIGHT_MOTOR_PWM, FRONT_RIGHT_MOTOR_EN1, FRONT_RIGHT_MOTOR_EN2,
    REAR_LEFT_MOTOR_PWM, REAR_LEFT_MOTOR_EN1, REAR_LEFT_MOTOR_EN2,
    REAR_RIGHT_MOTOR_PWM, REAR_RIGHT_MOTOR_EN1, REAR_RIGHT_MOTOR_EN2,
    50 
);

void CtrlCHandler(int s) {
    printf("Caught signal %d\n", s);
    movementCtrl.StopAll();
    movementCtrl.ResetPWM();
    exit(1);
}

int main(int argc, const char** argv) {
    // Initializing signals
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = CtrlCHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    std::cout << "Setting up" << std::endl;

    // Initializing joystick
    Joystick joystick("/dev/input/js0");
    if (!joystick.isFound()) {
        printf("Joy stick not found.\n");
        exit(1);
    }   

    for (;;) {
        delayMicroseconds(1000);
        JoystickEvent event;
        if (joystick.sample(&event)) {
            if (event.isButton()) {
                switch (event.number) {
                case 13:
                    if (event.value == 0) {
                        movementCtrl.StopAll();
                    } else {
                        movementCtrl.MoveForward(4096);
                    }
                    break;
                case 14:
                    if (event.value == 0) {
                        // Key up
                    } else {
                        // Key Down
                    }
                    break;
                }
            }
        }
    }
    return 0;
}
