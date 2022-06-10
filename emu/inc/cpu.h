#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <bitset>
#include "bus.h"

struct Cpu {
    uint8_t grf[4];
    uint8_t srf[8];
    uint8_t mip;
    std::bitset<8> flags;
    Bus bus;

    Cpu();

    void cycle();
};

#endif
