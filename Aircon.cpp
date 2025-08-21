#include <iostream>

#include "Aircon.h"

Aircon::Aircon(PulseController &pc): m_pc(pc) {}

void Aircon::invoke(std::vector<std::string> argv) {
    if (argv.size() == 0) {
        std::cerr << "USAGE: ./ir aircon <command>" << std::endl;
        return;
    }
    std::string command = argv[0];
    if (command == "send-status") {
        if (argv.size() <= 2) {
            std::cerr << "too few argument" << std::endl;
            return;
        }
        Status s;
        try {
            s = parse_arguments(std::vector(argv.begin() + 1, argv.end()));
        } catch (IRInvalidParamException &e) {
            std::cerr << e.what() << std::endl;
            return;
        }
        if (s.sp_mode == SpecialMode::silent) {
            transmit(SP_SILENT, false);
        } else if (s.sp_mode == SpecialMode::gentle) {
            transmit(SP_GENTLE, false);
        }
        transmit(construct_signal(s)); 
    } else if (command == "power-off") {
        if (argv.size() != 1) {
            std::cerr << "too much argument" << std::endl;
            return;
        }
        transmit(POWER_OFF);
    } else if (command == "toggle-swing") {
        if (argv.size() != 1) {
            std::cerr << "too much argument" << std::endl;
            return;
        }
        transmit(TOGGLE_SWING);
    } else if (command == "wind-angle") {
        if (argv.size() != 1) {
            std::cerr << "too much argument" << std::endl;
            return;
        }
        transmit(WIND_ANGLE);
    } else {
            std::cerr << "unknown command. available commands are: \n" 
            << "send-status" << std::endl;
    }
}

template<size_t N>
void Aircon::transmit(const uint8_t (&payload)[N], bool end, bool single) {
    // wrap array with vector
    std::vector v(std::begin(payload), std::end(payload));
    transmit(v, end, single);
}

void Aircon::transmit(const std::vector<uint8_t> payload, bool end, bool single) {
    std::vector v(std::begin(CUSTOMER_CODE), std::end(CUSTOMER_CODE));
    v.insert(v.end(), std::begin(payload), std::end(payload));
    // send twice basically
    m_pc.write(ir::TOSHIBA_AIRCON, v, single && end);
    if (single) { return; }
    m_pc.write_pulse(0, ir::TOSHIBA_AIRCON.T * 10);
    m_pc.write(ir::TOSHIBA_AIRCON, v, end);
}

Aircon::Status Aircon::parse_arguments(std::vector<std::string> params){
    Status status;
    // mode
    std::string mode_str = params[0];
    if (mode_str == "cooling") {
        status.mode = OpMode::cooling;
    } else if (mode_str == "dry") {
        status.mode = OpMode::dry;
    } else if (mode_str == "auto") {
        status.mode = OpMode::automatic;
    } else if (mode_str == "heating") {
        status.mode = OpMode::heating;
    } else {
        throw IRInvalidParamException("invalid operation mode " + mode_str);
    }

    // temp
    int8_t temp_val;
    try {
        temp_val = std::stoi(params[1]);
    } catch (std::exception &e) { // std::invalid_argument or std::out_of_range
        throw IRInvalidParamException("invalid temp value " + params[1]);
    }
    if (status.mode == OpMode::automatic) {
        if (temp_val < -5 || temp_val > 5) {
            throw IRInvalidParamException("invalid temp value " + params[1]);
        }
        status.temp_code = GRAY_ISH_CODE[temp_val + 8];
    } else {
        if (temp_val < 17 || temp_val > 30) {
            throw IRInvalidParamException("invalid temp value " + params[1]);
        }
        status.temp_code = GRAY_ISH_CODE[temp_val - 17];
    }
    
    // wind power
    if (status.mode == OpMode::automatic || status.mode == OpMode::dry) {
        // just discard third param
        status.power = WindPower::unspecifiable;
    } else {
        std::string power_str = params[2];
        if (power_str == "max") {
            status.power = WindPower::maximum;
        } else if (power_str == "mid") {
            status.power = WindPower::medium;
        } else if (power_str == "min") {
            status.power = WindPower::minimum;
        } else if (power_str == "auto") {
            status.power = WindPower::automatic; 
        } else {
            throw IRInvalidParamException("invalid power value " + power_str);
        }
    }

    // additional option
    if (params.size() < 4) {
        status.sp_mode = SpecialMode::none;
        return status;
    }
    
    if (status.mode == OpMode::dry) {
        throw IRInvalidParamException("you cannot specify other option in dry mode");
    }
    std::string sp_str = params[3];
    if (sp_str == "silent") {
        status.sp_mode = SpecialMode::silent;
    } else if (sp_str == "gentle") {
        status.sp_mode = SpecialMode::gentle;
    } else {
        throw IRInvalidParamException("invalid additional option " + sp_str);
    }
    // wind power is defautled to auto
    if (status.mode != OpMode::automatic) {
        status.power = WindPower::automatic;
    }
    if (params.size() >= 5) {
        throw IRInvalidParamException("too much arguments");
    }
    return status;
}

std::vector<uint8_t> Aircon::construct_signal(Aircon::Status status) {
    std::vector<uint8_t> signal(4);
    signal[0] = (enum_cast(status.power) << 5) | 0x1f;
    signal[1] = ~signal[0];
    signal[2] = (status.temp_code << 4) | (enum_cast(status.mode) << 2);
    signal[3] = ~signal[2];
    return signal;
}