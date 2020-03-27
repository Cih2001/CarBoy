#include <iostream>
#include <signal.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "joystick.hh"
#include <unistd.h>

#define EN1 0
#define IN1 2
#define IN2 3

#define EN2 26
#define IN3 27
#define IN4 28


void CtrlCHandler(int s) {
	printf("Caught signal %d\n", s);
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);

	exit(1);
}

int main(int argc, const char** argv) {
	// Initializing signals
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = CtrlCHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);


	// Setting up wiring Pi.
	std::cout << "Setting up" << std::endl;
	wiringPiSetup();

	int speed = 255;

	softPwmCreate(EN1, speed, 255);
	softPwmCreate(EN2, speed, 255);

	softPwmWrite(EN1, speed);
	softPwmWrite(EN2, speed);

	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW);

	

	for (;;);

	return 0;
}
