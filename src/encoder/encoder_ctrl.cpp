#include "encoder_ctrl.h"
#include "global.h"

#include <wiringPi.h>

Encoder::Encoder(unsigned int signal_pin_1, unsigned int signal_pin_2) {
    pinMode(signal_pin_1, INPUT);
    pinMode(signal_pin_2, INPUT);
    signal_pin_1_ = signal_pin_1;
    signal_pin_2_ = signal_pin_2;

    t_ = new std::thread(&Encoder::thread_entry, this);;
}

int Encoder::GetCounter() {
    return counter_;
}

void Encoder::thread_entry() {
    int last_state = digitalRead(signal_pin_1_);
    for (;;) {
        // read encoder values
        int stateA = digitalRead(signal_pin_1_); 
        int stateB = digitalRead(signal_pin_2_); 
        if (stateA != last_state) {
            if (stateB != stateA) {
                counter_++;
            } else {
                counter_--;
            }
            last_state = stateA;
        }
    }
}

EncoderController::EncoderController(
    unsigned int signal_pin_1_front_left,
    unsigned int signal_pin_2_front_left,
    unsigned int signal_pin_1_front_right,
    unsigned int signal_pin_2_front_right,
    unsigned int signal_pin_1_rear_left,
    unsigned int signal_pin_2_rear_left,
    unsigned int signal_pin_1_rear_right,
    unsigned int signal_pin_2_rear_right)
{
    encoders.push_back(new Encoder(signal_pin_1_front_left, signal_pin_2_front_left));
    encoders.push_back(new Encoder(signal_pin_1_front_right, signal_pin_2_front_right));
    encoders.push_back(new Encoder(signal_pin_1_rear_left, signal_pin_2_rear_left));
    encoders.push_back(new Encoder(signal_pin_1_rear_right, signal_pin_2_rear_right));
}

std::vector<int> EncoderController::GetCounters() {
    std::vector<int> result;
    result.push_back(encoders[0]->GetCounter());
    result.push_back(encoders[1]->GetCounter());
    result.push_back(encoders[2]->GetCounter());
    result.push_back(encoders[3]->GetCounter());
    return  result;
}
