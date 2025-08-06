#ifndef __IR_CEILING_H__
#define __IR_CEILING_H__

#include <cstdint>

#include "Format.h"
#include "PulseController.h"

class Ceiling {
public:
    Ceiling(PulseController &pc): m_pc(pc) {}
    
    // interface
    void invoke(std::vector<std::string> argv) {
        if (argv.size() != 1) {
            std::cerr << "USAGE: ./ir ceiling <command>" << std::endl;
            return;
        }
        std::string command = argv[0];
        if (command == "toggle") {
            transmit(TOGGLE); 
        } else if (command == "night") {
            transmit(NIGHT);
        } else if (command == "full") {
            transmit(FULL);
        } else if (command == "brighter") {
            transmit(BRIGHTER);
        } else if (command == "dimmer") {
            transmit(DIMMER);
        } else {
             std::cerr << "unknown command. available commands are: \n" 
                << "toggle, night, full, brighter, dimmer" << std::endl;
        }  
    }

private:
    template<size_t N>
    void transmit(const uint8_t (&payload)[N]){
        std::vector v(std::begin(CUSTOMER_CODE), std::end(CUSTOMER_CODE));
        v.insert(v.end(), std::begin(payload), std::end(payload));
        m_pc.write(ir::NEC_FORMAT, v);
    }

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