#ifndef __IR_AIRCON_H__
#define __IR_AIRCON_H__

#include <cstdint>
#include <exception>
#include <string>

#include "Format.h"
#include "PulseController.h"

template<typename E>
constexpr typename std::underlying_type<E>::type enum_cast(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

class IRInvalidParamException: std::exception {
public:
    IRInvalidParamException(std::string reason): m_reason(reason) {};
    const char* what() const noexcept override {
        return m_reason.c_str();
    }
private:
    std::string m_reason;
};

class Aircon {
public:
    Aircon(PulseController &pc);

    void invoke(std::vector<std::string> argv);

private:
    // some data types
    enum class OpMode: uint8_t {
        cooling = 0,
        dry,
        automatic, 
        heating
    };

    enum class WindPower: uint8_t {
        unspecifiable = 0,
        maximum = 1,
        medium = 2, 
        minimum = 4,
        automatic = 5
    };

    enum class SpecialMode: uint8_t {
        none = 0,
        silent = 1,
        gentle = 2  // maroyaka mode
    };

    struct Status {
        OpMode mode;
        WindPower power;
        uint8_t temp_code;
        SpecialMode sp_mode;
    };

    static constexpr uint8_t GRAY_ISH_CODE[] {
        0x0, 0x1, 0x3, 0x2, 0x6, 0x7, 0x5, 0x4, 0xc, 0xd, // it is ok so far but...
        0x9, 0x8, // it should be 0xf, 0xe
        0xa, 0xb  // this part is also ok
    };

    template <size_t N>
    void transmit(const uint8_t (&payload)[N], bool end = true, bool single = false);
    void transmit(const std::vector<uint8_t> payload, bool end = true, bool single = false);
    Aircon::Status parse_arguments(std::vector<std::string> params);
    std::vector<uint8_t> construct_signal(Aircon::Status status);

    // fixed value
    static constexpr uint8_t CUSTOMER_CODE[] {0xb2, 0x4d};
    
    // commands
    static constexpr uint8_t SP_SILENT[] {0xe0, 0x1f, 0x02, 0xfd};
    static constexpr uint8_t SP_GENTLE[] {0xe0, 0x1f, 0x03, 0xfc};

    static constexpr uint8_t POWER_OFF[] {0x7b, 0x84, 0xe0, 0x1f};
    static constexpr uint8_t WIND_ANGLE[] {0x0f, 0xf0, 0xe0, 0x1f};
    static constexpr uint8_t TOGGLE_SWING[] {0x6b, 0x94, 0xe0, 0x1f};

    PulseController &m_pc;
};

#endif