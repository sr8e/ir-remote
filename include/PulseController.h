#ifndef __IR_PULSECONTROLLER_H__
#define __IR_PULSECONTROLLER_H__

#include "Format.h"
#include <wiringPi.h>
#include <chrono>
#include <cstdint>
#include <vector>

class PulseController {
public:
    PulseController(int gpio);
    // convenient interface
    void write(ir::Format fmt, std::vector<uint8_t> data, bool end = true);
    // low-level control
    void write_pulse(int level, int dur);
private:
    
    int m_gpio;  // output pin (gpio)
};

#endif