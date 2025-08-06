#include <wiringPi.h>

#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include "PulseController.h"
#include "Ceiling.h"

void debug() {
    // exported arm peripheral clk/pwm memory region
    volatile uint32_t *reg_pwm = _wiringPiPwm; // base 0x20C000
    volatile uint32_t *reg_clk = _wiringPiClk; // base 0x101000
    std::cout << std::hex << "pwm ->0x" << reg_pwm << ", clk ->0x" << reg_clk;
    // CM_PWMCTL 0x1010a0 -> 40 x 4 byte 
    // CM_PWMDIV 0x1010a4 -> 41 x 4 byte 24bit fixed point value. integer 12 bit / fraction 12 bit
    std::cout << "PWMCTL_SRC -> 0x" << *(reg_clk + 40) << ", PWMDIV -> 0x" << *(reg_clk + 41) << std::endl;

    pwmWrite(13, 1);
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "USAGE: ./ir <device> <command> [argument...]" << std::endl;
        return 1;
    }
    
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "gpio setup failed" << std::endl;
        return -1;
    }
    
    PulseController pc(13);
    std::string device = argv[1];

    std::vector<std::string> args(argc - 2);
    for (int i = 0; i < argc - 2; i++) {
        args[i] = std::string(argv[i + 2]);
    }
    if (device == "ceiling") {
        Ceiling ceiling(pc);
        ceiling.invoke(args);
    } else if (device == "aircon") {

    } else if (device == "debug") {
        debug();
    } else {
        std::cerr << "unknown device. available devices are: \n" 
            << "ceiling, aircon" << std::endl;
    }

}