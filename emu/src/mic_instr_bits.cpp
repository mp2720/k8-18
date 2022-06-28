#include "../inc/mic_instr_bits.h"

void MicInstrBits::set_field(Bit start, size_t size, int value) {
    for (size_t i = 0; i < size; i++) {
        bits.set(start + i, value & 1);
        value >>= 1;
    }
}

int MicInstrBits::get_field(Bit start, size_t size) const {
    int value = 0;

    for (size_t i = size; i != 0; i--) {
        value <<= 1;
        value |= bits.test(start + i - 1) & 1;
    }

    return value;
}

std::vector<std::string> MicInstrBits::find_conflicts() const {
    auto conflicts = std::vector<std::string>();

    if (bits.test(CND)) {
        if (bits.test(GOE)
            || bits.test(GWE)
            || get_field(GWS0, 2)
            || get_field(GRS0, 2)
            || bits.test(RR)
            || bits.test(RL)
            || get_field(OP0, 2)
            || bits.test(INB)
            || bits.test(EC))
            conflicts.push_back("conditional NMIP is given with GOE, GWE, GWS, GRS, RR, RL, OP, INV or EC enabled");
    } else {
        if (bits.test(FEO) && (bits.test(INB) || bits.test(INA) || bits.test(GOE)
                               || bits.test(SOE)))
            conflicts.push_back("inverted output to C-bus from ALU and flags enabled");

        if (bits.test(RR) && bits.test(RL))
            conflicts.push_back("rotate C-bus to right and to left enabled");
    }

    if (bits.test(IIP) && bits.test(ISP))
        conflicts.push_back("IP and SP increment\\decrement enabled");

    if (bits.test(DS) && bits.test(CS))
        conflicts.push_back("data and code segments enabled");

    if (bits.test(WR) && bits.test(RD))
        conflicts.push_back("write and read external-bus enabled");

    return conflicts;
}
