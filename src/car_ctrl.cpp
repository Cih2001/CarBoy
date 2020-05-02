#include "global.h"
#include "car_ctrl.h"

#include <unistd.h>
#include <fcntl.h>

#include <cstring>
#include <algorithm>

#define PIPE_BUFFER_SIZE 256

CarController::CarController() {
    movementCtrl = std::make_unique<MovementController>(
        FRONT_LEFT_MOTOR_PWM, FRONT_LEFT_MOTOR_EN1, FRONT_LEFT_MOTOR_EN2,
        FRONT_RIGHT_MOTOR_PWM, FRONT_RIGHT_MOTOR_EN1, FRONT_RIGHT_MOTOR_EN2,
        REAR_LEFT_MOTOR_PWM, REAR_LEFT_MOTOR_EN1, REAR_LEFT_MOTOR_EN2,
        REAR_RIGHT_MOTOR_PWM, REAR_RIGHT_MOTOR_EN1, REAR_RIGHT_MOTOR_EN2,
        50 
    );

    encoderCtrl = std::make_unique<EncoderController>(
        FRONT_LEFT_ENCODER_PIN1,
        FRONT_LEFT_ENCODER_PIN2,
        FRONT_RIGHT_ENCODER_PIN1,
        FRONT_RIGHT_ENCODER_PIN2,
        REAR_LEFT_ENCODER_PIN1,
        REAR_LEFT_ENCODER_PIN2,
        REAR_RIGHT_ENCODER_PIN1,
        REAR_RIGHT_ENCODER_PIN2
    );

    // Initializing joystick
    // Creating a pipe.
    if (pipe(pipe_fd) == -1) {
        perror("Error creating pipe");
        exit(1);
    }
    int retval = fcntl( pipe_fd[0], F_SETFL, fcntl(pipe_fd[0], F_GETFL) | O_NONBLOCK);
    logController.printf("Ret from fcntl: %d\n", retval);
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
}

void CarController::StopAll() {
    movementCtrl->StopAll();
    movementCtrl->ResetPWM();
}

void CarController::StartControl() {
    int relativeSpeed = 0;
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
                movementCtrl->SetRelativeSpeed(relativeSpeed);
                break;
            case CMD_MOVE_FORWARD:
                movementCtrl->MoveForwardRelative(relativeSpeed);
                break;
            case CMD_MOVE_BACKWARD:
                movementCtrl->MoveBackwardRelative(relativeSpeed);
                break;
            case CMD_STOP:
            default:
                movementCtrl->StopAll();
            }
        }

        // auto counters =  encoderCtrl->GetCounters();
        auto speeds =  encoderCtrl->GetSpeeds();
        logController.updateEncoderSpeed(0, speeds[0]);
        logController.updateEncoderSpeed(1, speeds[1]);

    }
}
