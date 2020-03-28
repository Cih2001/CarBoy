#ifndef MovementCtrl
#define MovementCtrl

#define ENGINE_FRONT_LEFT 0
#define ENGINE_FRONT_RIGHT 1
#define ENGINE_REAR_LEFT 2
#define ENGINE_REAR_RIGHT 3

#define PCA_PIN_BASE 300 // This is the base number for the extended pins on WiringPi by PCA9685
#define MAX_PWM 255 // PWN value can be [0..255]

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

    void StopAll();
    void ResetPWM();

private:
    int _pca_fd;
    unsigned int _pwmFrequency;
    unsigned int _enginesPwmPins[4];
    unsigned int _enginesEN1Pins[4];
    unsigned int _enginesEN2Pins[4];
    int _enginesSpeed[4];

    // Engine state can be stopped or moving forward/backward.
    enum EngineState {
       ENGINE_STOPPED, MOVING_FORWARD, MOVING_BACKWARD
    };

    EngineState _enginesState[4];

    // methods:
    void moveEngine(unsigned int idx, int speed, MovementDirection dir);
};
#endif

