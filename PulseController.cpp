#include "PulseController.h"

#include <wiringPi.h>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono;

int64_t usecond(system_clock::duration t) {
    return duration_cast<microseconds>(t).count();
}

void spin_lock(system_clock::time_point ref, int64_t dur_us) {
    while (usecond(system_clock::now() - ref) < dur_us);
}

PulseController::PulseController(int gpio):m_gpio(gpio) {
    pinMode(gpio, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    /* 
     * Pi 4 has 54 MHz oscillator, but wiringPi automatically convert
     * as if it is working on 19.2 MHz (like Pi 3).
     * 19.2M / 38k ~= 505
     * as we need just 1/3 duty, range=3 is enough
     * so clock is 505 / 3 ~= 168
     */
    pwmSetClock(168);
    pwmSetRange(3);
    pwmWrite(m_gpio, 0);
    // wait for gpio is ready (really?)
    std::this_thread::sleep_for(milliseconds(10));
}

void PulseController::write(ir::Format fmt, std::vector<uint8_t> data) {
    // leader
    auto t = system_clock::now();
    write_pulse(1, fmt.T * fmt.leader_on);
    write_pulse(0, fmt.T * fmt.leader_off);
    std::cout << "header dur -> " << usecond(system_clock::now() - t) << std::endl;

    // dump all the data
    for(uint8_t byte: data) {
        for(int i = 0; i < 8; i++) {
            int shift = fmt.lsb_first ? i : (7 - i);
            if ((byte >> i) & 1) {
                write_pulse(1, fmt.T * fmt.data_1_on);
                write_pulse(0, fmt.T * fmt.data_1_off);
            } else {
                write_pulse(1, fmt.T * fmt.data_0_on);
                write_pulse(0, fmt.T * fmt.data_0_off);
            }
        }
    }

    // stop bit & wait for enough time.
    write_pulse(1, fmt.T * fmt.stop);
    pwmWrite(m_gpio, 0);
    std::this_thread::sleep_for(milliseconds(150));
}

void PulseController::write_pulse(int level, int dur_us) {
    auto t = system_clock::now();
    pwmWrite(m_gpio, level);
    spin_lock(t, dur_us);
}