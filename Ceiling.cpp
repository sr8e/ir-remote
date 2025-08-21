#include <iostream>

#include "Ceiling.h"

Ceiling::Ceiling(PulseController &pc): m_pc(pc) {}

void Ceiling::invoke(std::vector<std::string> argv) {
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

template<size_t N>
void Ceiling::transmit(const uint8_t (&payload)[N]){
    std::vector v(std::begin(CUSTOMER_CODE), std::end(CUSTOMER_CODE));
    v.insert(v.end(), std::begin(payload), std::end(payload));
    m_pc.write(ir::NEC_FORMAT, v);
}