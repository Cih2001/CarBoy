#ifndef CARBOY_CAR_CTRL_H_
#define CARBOY_CAR_CTRL_H_

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

#define FRONT_LEFT_ENCODER_PIN1 23
#define FRONT_LEFT_ENCODER_PIN2 24

#define FRONT_RIGHT_ENCODER_PIN1 27
#define FRONT_RIGHT_ENCODER_PIN2 28

#define REAR_LEFT_ENCODER_PIN1 23
#define REAR_LEFT_ENCODER_PIN2 23

#define REAR_RIGHT_ENCODER_PIN1 23
#define REAR_RIGHT_ENCODER_PIN2 23

#include "movement/movement_ctrl.h"
#include "encoder/encoder_ctrl.h"
#include "joystick/joystick_ctrl.h"

#include <memory>

class CarController {
public:
    CarController();

    void StartControl();

    void StopAll();
private:
    std::unique_ptr<MovementController> movementCtrl = nullptr; 
    std::unique_ptr<EncoderController> encoderCtrl = nullptr;
    int pipe_fd[2];
};
#endif
