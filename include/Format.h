#ifndef __IR_FORMAT_H__
#define __IR_FORMAT_H__

namespace ir {
    class Format {
    public:
        int T; // unit pulse width
        int leader_on; // leader on/off duration (in unit T)
        int leader_off;
        int data_0_on;  // on/off duration to represent data 0/1
        int data_0_off;
        int data_1_on;
        int data_1_off;
        int stop; // stop bit duration
        bool lsb_first; // bit order
    };

    static constexpr Format NEC_FORMAT{562, 16, 8, 1, 1, 1, 3, 1, true};
    static constexpr Format TOSHIBA_AIRCON{490, 9, 9, 1, 1, 1, 3, 1, false};
}

#endif