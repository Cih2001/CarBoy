#ifndef CARBOY_CAR_CTRL_H_
#define CARBOY_CAR_CTRL_H_

#include "movement/movement_ctrl.h"

#include <memory>

class CarController {
public:
    CarController();
private:
    std::unique_ptr<MovementController> movementCtrl = nullptr; 
};
#endif
