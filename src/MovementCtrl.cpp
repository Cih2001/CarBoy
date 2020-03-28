#include "MovementCtrl.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <pca9685.h>
#include <iostream>

MovementController::MovementController(
    const unsigned int flPwm, const unsigned int flEN1, const unsigned int flEN2,         
    const unsigned int frPwm, const unsigned int frEN1, const unsigned int frEN2,         
    const unsigned int rlPwm, const unsigned int rlEN1, const unsigned int rlEN2,         
    const unsigned int rrPwm, const unsigned int rrEN1, const unsigned int rrEN2,
    const unsigned int pwmFrequency
    ) {
    // Initializing private variables.
    this->_enginesPwmPins[ENGINE_FRONT_LEFT] = flPwm;
    this->_enginesEN1Pins[ENGINE_FRONT_LEFT] = flEN1;
    this->_enginesEN2Pins[ENGINE_FRONT_LEFT] = flEN2;

    this->_enginesPwmPins[ENGINE_FRONT_RIGHT] = frPwm;
    this->_enginesEN1Pins[ENGINE_FRONT_RIGHT] = frEN1;
    this->_enginesEN2Pins[ENGINE_FRONT_RIGHT] = frEN2;

    this->_enginesPwmPins[ENGINE_REAR_LEFT] = rlPwm;
    this->_enginesEN1Pins[ENGINE_REAR_LEFT] = rlEN1;
    this->_enginesEN2Pins[ENGINE_REAR_LEFT] = rlEN2;

    this->_enginesPwmPins[ENGINE_REAR_RIGHT] = rrPwm;
    this->_enginesEN1Pins[ENGINE_REAR_RIGHT] = rrEN1;
    this->_enginesEN2Pins[ENGINE_REAR_RIGHT] = rrEN2;

    this->_pwmFrequency = pwmFrequency;
    
    for (unsigned int i = 0; i < 4; i++) {
        // Setting engines speed.
        this->_enginesSpeed[i] = 0;
        // Setting engines state. they are not moving.
        this->_enginesState[i] = ENGINE_STOPPED;
    }
	// Setting up wiring Pi.
	wiringPiSetup();

	// Setting up PCA9685
	_pca_fd = pca9685Setup(PCA_PIN_BASE, 0x40, _pwmFrequency);
	if (_pca_fd < 0) {
		printf("Error setting up PCA9685\n");
		exit(1);
	}

	pca9685PWMReset(_pca_fd);

    return;
}

void MovementController::SetSpeedAll(int speed) {
    for (unsigned int i = 0; i < 4; i++) {
        // Setting engines speed.
        _enginesSpeed[i] = speed;
    }

    return;
}

void MovementController::MoveForward(int speed = 0) {
    // update speed.
    _enginesSpeed[ENGINE_FRONT_LEFT] = speed;
    _enginesSpeed[ENGINE_FRONT_RIGHT] = speed;
    _enginesSpeed[ENGINE_REAR_LEFT] = speed;
    _enginesSpeed[ENGINE_REAR_RIGHT] = speed;

    // update directions.
    _enginesState[ENGINE_FRONT_LEFT] = MOVING_FORWARD;
    _enginesState[ENGINE_FRONT_RIGHT] = MOVING_FORWARD;
    _enginesState[ENGINE_REAR_LEFT] = MOVING_FORWARD;
    _enginesState[ENGINE_REAR_RIGHT] = MOVING_FORWARD;

	pwmWrite(
        PCA_PIN_BASE + _enginesPwmPins[ENGINE_FRONT_LEFT],
        _enginesSpeed[ENGINE_FRONT_LEFT]
    );
	pwmWrite(
        PCA_PIN_BASE + _enginesPwmPins[ENGINE_FRONT_RIGHT],
        _enginesSpeed[ENGINE_FRONT_RIGHT]
    );
	pwmWrite(
        PCA_PIN_BASE + _enginesPwmPins[ENGINE_REAR_LEFT],
        _enginesSpeed[ENGINE_REAR_LEFT]
    );
	pwmWrite(
        PCA_PIN_BASE + _enginesPwmPins[ENGINE_REAR_RIGHT],
        _enginesSpeed[ENGINE_REAR_RIGHT]
    );

    digitalWrite(PCA_PIN_BASE + _enginesEN1Pins[ENGINE_FRONT_LEFT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN2Pins[ENGINE_FRONT_LEFT], LOW);
    digitalWrite(PCA_PIN_BASE + _enginesEN1Pins[ENGINE_FRONT_RIGHT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN2Pins[ENGINE_FRONT_RIGHT], LOW);
    digitalWrite(PCA_PIN_BASE + _enginesEN1Pins[ENGINE_REAR_LEFT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN2Pins[ENGINE_REAR_LEFT], LOW);
    digitalWrite(PCA_PIN_BASE + _enginesEN1Pins[ENGINE_REAR_RIGHT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN2Pins[ENGINE_REAR_RIGHT], LOW);

    return;
}

void MovementController::moveEngine(unsigned int idx, int speed, MovementDirection dir)
{
    return;
}

void MovementController::StopAll() {
    // update directions.
    _enginesState[ENGINE_FRONT_LEFT] = ENGINE_STOPPED;
    _enginesState[ENGINE_FRONT_RIGHT] = ENGINE_STOPPED;
    _enginesState[ENGINE_REAR_LEFT] = ENGINE_STOPPED;
    _enginesState[ENGINE_REAR_RIGHT] = ENGINE_STOPPED;
    
    digitalWrite(PCA_PIN_BASE + _enginesEN1Pins[ENGINE_FRONT_LEFT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN2Pins[ENGINE_FRONT_LEFT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN1Pins[ENGINE_FRONT_RIGHT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN2Pins[ENGINE_FRONT_RIGHT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN1Pins[ENGINE_REAR_LEFT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN2Pins[ENGINE_REAR_LEFT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN1Pins[ENGINE_REAR_RIGHT], HIGH);
    digitalWrite(PCA_PIN_BASE + _enginesEN2Pins[ENGINE_REAR_RIGHT], HIGH);
}

void MovementController::ResetPWM() {
	pca9685PWMReset(_pca_fd);
}
