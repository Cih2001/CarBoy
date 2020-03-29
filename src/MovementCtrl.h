#ifndef MovementCtrl
#define MovementCtrl

#define FRONT_LEFT_MOTOR 0
#define FRONT_RIGHT_MOTOR 1
#define REAR_LEFT_MOTOR 2
#define REAR_RIGHT_MOTOR 3

#define PCA_PIN_BASE 300 // This is the base number for the extended pins on WiringPi by PCA9685
#define MAX_PWM 255 // PWN value can be [0..255]

#define NUM_OF_MOTORS 4

enum MovementDirection {
    FORWARD, BACKWARD
};


class MovementController {
public:

    MovementController(
        const unsigned int flPwm, const unsigned int flEN1, const unsigned int flEN2,         
        const unsigned int frPwm, const unsigned int frEN1, const unsigned int frEN2,         
        const unsigned int rlPwm, const unsigned int rlEN1, const unsigned int rlEN2,         
        const unsigned int rrPwm, const unsigned int rrEN1, const unsigned int rrEN2,
        const unsigned int pwmFrequency
    );

    void SetSpeedAll(int speed);
    void MoveForward(int speed);
    void MoveBackward(int speed);

    void StopAll();
    void ResetPWM();

private:
    int _pca_fd;
    unsigned int _pwmFrequency;
    unsigned int _motorsPwmPins[NUM_OF_MOTORS];
    unsigned int _motorsEN1Pins[NUM_OF_MOTORS];
    unsigned int _motorsEN2Pins[NUM_OF_MOTORS];
    int _motorsSpeed[NUM_OF_MOTORS];

    // Motor state can be stopped or moving forward/backward.
    enum MotorState {
       MOTOR_STOPPED, MOVING_FORWARD, MOVING_BACKWARD
    };

    MotorState _motorsState[NUM_OF_MOTORS];

    // methods:
    void moveMotor(const unsigned int idx, int speed, MovementDirection dir);
    void stopMotor(const unsigned int idx);
};
#endif

