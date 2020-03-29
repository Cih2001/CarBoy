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
    this->_motorsPwmPins[FRONT_LEFT_MOTOR] = flPwm;
    this->_motorsEN1Pins[FRONT_LEFT_MOTOR] = flEN1;
    this->_motorsEN2Pins[FRONT_LEFT_MOTOR] = flEN2;

    this->_motorsPwmPins[FRONT_RIGHT_MOTOR] = frPwm;
    this->_motorsEN1Pins[FRONT_RIGHT_MOTOR] = frEN1;
    this->_motorsEN2Pins[FRONT_RIGHT_MOTOR] = frEN2;

    this->_motorsPwmPins[REAR_LEFT_MOTOR] = rlPwm;
    this->_motorsEN1Pins[REAR_LEFT_MOTOR] = rlEN1;
    this->_motorsEN2Pins[REAR_LEFT_MOTOR] = rlEN2;

    this->_motorsPwmPins[REAR_RIGHT_MOTOR] = rrPwm;
    this->_motorsEN1Pins[REAR_RIGHT_MOTOR] = rrEN1;
    this->_motorsEN2Pins[REAR_RIGHT_MOTOR] = rrEN2;

    this->_pwmFrequency = pwmFrequency;
    
    for (unsigned int i = 0; i < NUM_OF_MOTORS; i++) {
        // Setting motors speed.
        this->_motorsSpeed[i] = 0;
        // Setting motors state. they are not moving.
        this->_motorsState[i] = MOTOR_STOPPED;
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
    for (unsigned int i = 0; i < NUM_OF_MOTORS; i++) {
        // Setting motors speed.
        _motorsSpeed[i] = speed;
    }

    return;
}

void MovementController::MoveForward(int speed = 0) {
    this->moveMotor(REAR_LEFT_MOTOR,speed,FORWARD);
    this->moveMotor(REAR_RIGHT_MOTOR,speed,FORWARD);
    this->moveMotor(FRONT_LEFT_MOTOR,speed,FORWARD);
    this->moveMotor(FRONT_RIGHT_MOTOR,speed,FORWARD);
    return;
}

void MovementController::MoveBackward(int speed = 0) {
    moveMotor(REAR_LEFT_MOTOR,speed,BACKWARD);
    moveMotor(REAR_RIGHT_MOTOR,speed,BACKWARD);
    moveMotor(FRONT_LEFT_MOTOR,speed,BACKWARD);
    moveMotor(FRONT_RIGHT_MOTOR,speed,BACKWARD);
    return;
}

void MovementController::moveMotor(const unsigned int idx, int speed, MovementDirection dir)
{
    if (idx >= NUM_OF_MOTORS ) {
        return;
    }
    
    if (dir == FORWARD) {
        // Move motor forward:
        // Setting speed
        _motorsSpeed[idx] = speed;
        pwmWrite(
            PCA_PIN_BASE + _motorsPwmPins[idx],
            _motorsSpeed[idx]
        );
        // Setting direction.
        _motorsState[idx] = MOVING_FORWARD;
        digitalWrite(PCA_PIN_BASE + _motorsEN1Pins[idx], HIGH);
        digitalWrite(PCA_PIN_BASE + _motorsEN2Pins[idx], LOW);
    } else {
        // Move motor backward:
        // Setting speed
        _motorsSpeed[idx] = speed;
        pwmWrite(
            PCA_PIN_BASE + _motorsPwmPins[idx],
            _motorsSpeed[idx]
        );
        // Setting direction.
        _motorsState[idx] = MOVING_BACKWARD;
        digitalWrite(PCA_PIN_BASE + _motorsEN1Pins[idx], LOW);
        digitalWrite(PCA_PIN_BASE + _motorsEN2Pins[idx], HIGH);
    }
    return;
}

void MovementController::stopMotor(const unsigned int idx) {
    if (idx >= NUM_OF_MOTORS ) {
        return;
    }
    _motorsState[idx] = MOTOR_STOPPED;
    digitalWrite(PCA_PIN_BASE + _motorsEN1Pins[idx], HIGH);
    digitalWrite(PCA_PIN_BASE + _motorsEN2Pins[idx], HIGH);
}

void MovementController::StopAll() {
    stopMotor(FRONT_LEFT_MOTOR);
    stopMotor(FRONT_RIGHT_MOTOR);
    stopMotor(REAR_LEFT_MOTOR);
    stopMotor(REAR_RIGHT_MOTOR);
    return;
}

void MovementController::ResetPWM() {
	pca9685PWMReset(_pca_fd);
}
