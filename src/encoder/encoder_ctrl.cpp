#include "encoder_ctrl.h"
#include "global.h"

#include <wiringPi.h>

#define QUEUE_SIZE 15

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

void Encoder::addTimeToQueue(std::chrono::high_resolution_clock::time_point point) {
    mtx_.lock();
    time_queue_.push_back(point);
    if (time_queue_.size() > QUEUE_SIZE) {
        time_queue_.pop_front();
    }
    mtx_.unlock();
}

void Encoder::thread_entry() {

    auto last_state = digitalRead(signal_pin_1_);
    for (;;) {
        // read encoder values
        auto stateA = digitalRead(signal_pin_1_); 
        auto stateB = digitalRead(signal_pin_2_); 
        if (stateA != last_state) {
            this->addTimeToQueue(std::chrono::high_resolution_clock::now());
            if (stateB != stateA) {
                counter_++;
            } else {
                counter_--;
            }
            // compute speed.
            last_state = stateA;
        }
    }
}

float Encoder::GetSpeed() {
    mtx_.lock();
    float speed = 0.0;
    auto size = time_queue_.size();
    if ( size >= 2 ) {
        // Ensuring that enough points are captured.
        for (unsigned int i = 0; i < size - 1; i++) {
            std::chrono::duration<float, std::milli> dur = time_queue_[i + 1] - time_queue_[i];
            speed += 1000 / dur.count();
        }
        speed = speed / (size - 1);
    }
    mtx_.unlock();
    return speed;
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
