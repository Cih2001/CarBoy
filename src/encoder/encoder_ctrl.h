#ifndef CARBOY_ENCODER_ENCODER_CTRL_H_
#define CARBOY_ENCODER_ENCODER_CTRL_H_

#include <vector>
#include <memory>
#include <thread>

class Encoder {
public:
    explicit Encoder(unsigned int signal_pin_1, unsigned int signal_pin_2);

    int GetCounter();

private:
    unsigned int signal_pin_1_;
    unsigned int signal_pin_2_;
    int counter_ = 0;
    std::thread* t_ = nullptr;

    void thread_entry();
};

class EncoderController {
public:
    explicit EncoderController(
        unsigned int signal_pin_1_front_left,
        unsigned int signal_pin_2_front_left,
        unsigned int signal_pin_1_front_right,
        unsigned int signal_pin_2_front_right,
        unsigned int signal_pin_1_rear_left,
        unsigned int signal_pin_2_rear_left,
        unsigned int signal_pin_1_rear_right,
        unsigned int signal_pin_2_rear_right
    );

    std::vector<int> GetCounters();
private:
    std::vector<Encoder*> encoders;

};


#endif

