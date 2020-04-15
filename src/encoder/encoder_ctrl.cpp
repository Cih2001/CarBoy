#include "encoder_ctrl.h"
#include "global.h"

#include <wiringPi.h>
#include <chrono>


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

    auto last_state = digitalRead(signal_pin_1_);
    auto last_time = std::chrono::high_resolution_clock::now();
    for (;;) {
        // read encoder values
        auto stateA = digitalRead(signal_pin_1_); 
        auto stateB = digitalRead(signal_pin_2_); 
        if (stateA != last_state) {
            auto time = std::chrono::high_resolution_clock::now();
            if (stateB != stateA) {
                counter_++;
            } else {
                counter_--;
            }
            // compute speed.
            std::chrono::duration<float, std::milli> dur = time - last_time;
            speed_ = 1 / dur.count() * 1000;
            last_state = stateA;
            last_time = time;
        }
    }
}

float Encoder::GetSpeed() {
    return speed_;
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
    //encoders.push_back(new Encoder(signal_pin_1_front_right, signal_pin_2_front_right));
    //encoders.push_back(new Encoder(signal_pin_1_rear_left, signal_pin_2_rear_left));
    //encoders.push_back(new Encoder(signal_pin_1_rear_right, signal_pin_2_rear_right));
}

std::vector<int> EncoderController::GetCounters() {
    std::vector<int> result;
    result.push_back(encoders[0]->GetCounter());
    //result.push_back(encoders[1]->GetCounter());
    //result.push_back(encoders[2]->GetCounter());
    //result.push_back(encoders[3]->GetCounter());
    return  result;
}

std::vector<float> EncoderController::GetSpeeds() {
    std::vector<float> result;
    result.push_back(encoders[0]->GetSpeed());
    //result.push_back(encoders[1]->GetCounter());
    //result.push_back(encoders[2]->GetCounter());
    //result.push_back(encoders[3]->GetCounter());
    return  result;
}
