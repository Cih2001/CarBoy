#include "global.h"
#include "car_ctrl.h"

#include <signal.h>
#include <wiringPi.h>

#include <iostream>
#include <memory>



std::unique_ptr<CarController> carController = nullptr;

void CtrlCSignalHandler(int s) {
    logController.printf("Caught signal %d\n", s);
    carController->StopAll();
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

	// Setting up wiring Pi.
	wiringPiSetup();

    // Setting up car controller.
    carController = std::make_unique<CarController>();

    // Starting the car controller
    // this method wont return.
    carController->StartControl();

    return 0;
}
