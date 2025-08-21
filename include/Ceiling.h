#ifndef __IR_CEILING_H__
#define __IR_CEILING_H__

#include <cstdint>
#include <string>

#include "Format.h"
#include "PulseController.h"

class Ceiling {
public:
    Ceiling(PulseController &pc);
    
    // interface
    void invoke(std::vector<std::string> argv);

private:
    template<size_t N>
    void transmit(const uint8_t (&payload)[N]);

    // fixed value
    static constexpr uint8_t CUSTOMER_CODE[] {0x82, 0x6d};
    
    // commands
    static constexpr uint8_t TOGGLE[] {0xab, 0x54};
    static constexpr uint8_t NIGHT[] {0xbc, 0x43};
    static constexpr uint8_t FULL[] {0xa6, 0x59};
    static constexpr uint8_t BRIGHTER[] {0xba, 0x45};
    static constexpr uint8_t DIMMER[] {0xbb, 0x44};

    PulseController &m_pc;
};

#endif