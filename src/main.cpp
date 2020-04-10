#include "global.h"
#include "movement/movement_ctrl.h"
#include "joystick/joystick_ctrl.h"

#include <unistd.h>
#include <signal.h>
#include <wiringPi.h>

#include <iostream>
#include <memory>
#include <cstring>

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

#define PIPE_BUFFER_SIZE 256

MovementController movementCtrl(
    FRONT_LEFT_MOTOR_PWM, FRONT_LEFT_MOTOR_EN1, FRONT_LEFT_MOTOR_EN2,
    FRONT_RIGHT_MOTOR_PWM, FRONT_RIGHT_MOTOR_EN1, FRONT_RIGHT_MOTOR_EN2,
    REAR_LEFT_MOTOR_PWM, REAR_LEFT_MOTOR_EN1, REAR_LEFT_MOTOR_EN2,
    REAR_RIGHT_MOTOR_PWM, REAR_RIGHT_MOTOR_EN1, REAR_RIGHT_MOTOR_EN2,
    50 
);

void CtrlCSignalHandler(int s) {
    logController.printf("Caught signal %d\n", s);
    movementCtrl.StopAll();
    movementCtrl.ResetPWM();
    exit(1);
}

int main(int argc, const char** argv) {
    // Initializing signals
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = CtrlCSignalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    logController.printf("Initializing %s\n", "carboy");

    // Initializing joystick
    // Creating a pipe.
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Error creating pipe");
        exit(1);
    }
    JoystickController joystickCtrl(
        "/dev/input/js0",
        pipe_fd[1]
    );

    auto pid = fork();
    if (pid == -1) {
        perror("Error forking processes.");
        exit(1);
    }
    if ( pid == 0 ) {
        // child process.
        while ( joystickCtrl.Start() < 0) {
            logController.printf("Error starting joystick.\n");
            sleep(1);
        }
    }

    int relativeSpeed = 0;
    // Parent process continues here.
    for (;;) {
        // We should constantly read the pipe.

        char buffer[PIPE_BUFFER_SIZE];
        auto bytes_read = read(pipe_fd[0], buffer, PIPE_BUFFER_SIZE-1);

        JoystickCommandEvent event;
        if (bytes_read == sizeof(event)) {
            memcpy(&event, buffer, sizeof(event));
            switch (event.CommandCode) {
            case CMD_SET_SPEED:
                relativeSpeed = event.GlobalSpeedInPercent;
                logController.printf("Setting speed: %d\n", relativeSpeed);
                movementCtrl.SetRelativeSpeed(relativeSpeed);
                break;
            case CMD_MOVE_FORWARD:
                movementCtrl.MoveForwardRelative(relativeSpeed);
                break;
            case CMD_MOVE_BACKWARD:
                movementCtrl.MoveBackwardRelative(relativeSpeed);
                break;
            case CMD_STOP:
            default:
                movementCtrl.StopAll();
            }
        }
    }
    return 0;
}
