#include <iostream>
#include <signal.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "joystick.h"
#include <pca9685.h>


#define PCA_PIN_BASE 300 // This is the base number for the extended pins on WiringPi by PCA9685
#define MAX_PWM 255 // PWN value can be [0..255]
#define PWM_HERTZ 50 // PWM frequency

#define FRONT_RIGHT_MOTOR_PWM 13
#define FRONT_RIGHT_MOTOR_EN1 11
#define FRONT_RIGHT_MOTOR_EN2 12

#define FRONT_LEFT_MOTOR_PWM 0
#define FRONT_LEFT_MOTOR_EN1 1
#define FRONT_LEFT_MOTOR_EN2 2


int pca_fd;

void CtrlCHandler(int s) {
	printf("Caught signal %d\n", s);
	pca9685PWMReset(pca_fd);
	exit(1);
}

int main(int argc, const char** argv) {
	// Initializing signals
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = CtrlCHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	// Initializing joystick
	Joystick joystick("/dev/input/js0");
	if (!joystick.isFound()) {
		printf("Joy stick not found.\n");
		exit(1);
	}	

	// Setting up wiring Pi.
	std::cout << "Setting up" << std::endl;
	wiringPiSetup();

	// Setting up PCA9685
	pca_fd = pca9685Setup(PCA_PIN_BASE, 0x40, PWM_HERTZ);
	if (pca_fd < 0) {
		printf("Error setting up PCA9685\n");
		exit(1);
	}

	pca9685PWMReset(pca_fd);

	int speed = 2048;
	pwmWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_PWM, speed);
	pwmWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_PWM, speed);
	
	bool forward = false, backward = false;

	for (;;) {
		delayMicroseconds(1000);
		JoystickEvent event;
		if (joystick.sample(&event)) {
			if (event.isButton()) {
				switch (event.number) {
				case 13:
					if (event.value == 0) {
						// Key up
						if (forward) {
							digitalWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_EN1, LOW);
							digitalWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_EN2, LOW);
							digitalWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_EN1, LOW);
							digitalWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_EN2, LOW);
							forward = false;
						}
					} else {
						// Key Down
						if (!forward) {
							digitalWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_EN1, HIGH);
							digitalWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_EN2, LOW);
							digitalWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_EN1, HIGH);
							digitalWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_EN2, LOW);
							forward = true;
						}
					}
					break;
				case 14:
					if (event.value == 0) {
						// Key up
						digitalWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_EN1, LOW);
						digitalWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_EN2, LOW);
						digitalWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_EN1, LOW);
						digitalWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_EN2, LOW);
					} else {
						// Key Down
						digitalWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_EN1, LOW);
						digitalWrite(PCA_PIN_BASE + FRONT_RIGHT_MOTOR_EN2, HIGH);
						digitalWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_EN1, LOW);
						digitalWrite(PCA_PIN_BASE + FRONT_LEFT_MOTOR_EN2, HIGH);
					}
					break;
				}
			}
		}
	}
	return 0;
}
