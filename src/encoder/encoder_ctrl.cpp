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

void Encoder::addTimeToQueue(int counter, std::chrono::high_resolution_clock::time_point time) {
    mtx_.lock();
    Point point = {counter, time};
    points_queue_.push_back(point);
    if (points_queue_.size() > QUEUE_SIZE) {
        points_queue_.pop_front();
    }
    mtx_.unlock();
    // auto speed = GetSpeed();
    // std::ofstream debug_pipe;
    // debug_pipe.open("/tmp/fifo1", std::ios::out | std::ios::app);
    // debug_pipe << time.time_since_epoch().count() << " ," << counter <<
    //     " ," << speed << "\n";
    // debug_pipe.close();
}

void Encoder::thread_entry() {

    auto last_state = digitalRead(signal_pin_1_);
    auto last_time = std::chrono::high_resolution_clock::now();
    for (;;) {
        // read encoder values
        auto time = std::chrono::high_resolution_clock::now();
        auto stateA = digitalRead(signal_pin_1_); 
        auto stateB = digitalRead(signal_pin_2_); 
        if (stateA != last_state) {
            if (stateB != stateA) {
                counter_++;
            } else {
                counter_--;
            }
            // compute speed.
            last_state = stateA;
            // Lets take samples each milliseconds.
            last_time = time;
            this->addTimeToQueue(counter_, time);
            continue;
        }
        std::chrono::duration<float, std::milli> dur = time - last_time;
        if (dur.count() >= 15) {
            last_time = time;
            this->addTimeToQueue(counter_, time);
        }
    }
}

float Encoder::GetSpeed() {
    mtx_.lock();
    float speed = 0.0;
    auto size = points_queue_.size();
    if ( size >= 2 ) {
        // Ensuring that enough points are captured.
        std::chrono::duration<float, std::milli> dur 
            = points_queue_[size-1].time - points_queue_[0].time;
        auto delta_count = points_queue_[size-1].counter - points_queue_[0].counter;
        speed = delta_count / dur.count() * 1000;
    }
    mtx_.unlock();
    return speed;
}

// This is taking average method.
// float Encoder::GetSpeed() {
//     mtx_.lock();
//     float speed = 0.0;
//     auto size = points_queue_.size();
//     if ( size >= 2 ) {
//         // Ensuring that enough points are captured.
//         for (unsigned int i = 0; i < size - 1; i++) {
//             std::chrono::duration<float, std::milli> dur 
//                 = points_queue_[i + 1].time - points_queue_[i].time;
//             speed += (points_queue_[i + 1].counter - points_queue_[i].counter) *
//                 1000 / dur.count();
//         }
//         speed = speed / (size - 1);
//     }
//     mtx_.unlock();
//     return speed;
// }

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
    //encoders.push_back(new Encoder(signal_pin_1_rear_left, signal_pin_2_rear_left));
    //encoders.push_back(new Encoder(signal_pin_1_rear_right, signal_pin_2_rear_right));
}

std::vector<int> EncoderController::GetCounters() {
    std::vector<int> result;
    result.push_back(encoders[0]->GetCounter());
    result.push_back(encoders[1]->GetCounter());
    //result.push_back(encoders[2]->GetCounter());
    //result.push_back(encoders[3]->GetCounter());
    return  result;
}

// This returns speed in countrs per second.
std::vector<float> EncoderController::GetSpeeds() {
    std::vector<float> result;
    result.push_back(encoders[0]->GetSpeed());
    result.push_back(encoders[1]->GetSpeed());
    //result.push_back(encoders[2]->GetSpeed());
    //result.push_back(encoders[3]->GetSpeed());
    return  result;
}

// This return speed in RPM.
std::vector<float> EncoderController::GetSpeedsInRPM() {
    std::vector<float> result;
    auto speeds = this->GetSpeeds();
    result.push_back(speeds[0] / 65 * 60);
    result.push_back(speeds[1] / 65 * 60);
    // result.push_back(speeds[2] / 65 * 60);
    // result.push_back(speeds[3] / 65 * 60);
    return result;
}
