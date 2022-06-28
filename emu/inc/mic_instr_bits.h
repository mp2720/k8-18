#ifndef CTRL_BITS_H
#define CTRL_BITS_H

#include <bitset>
#include <vector>
#include <string>

enum Bit {
    /** GRF output enable. */
    GOE = 0,
    /** GRF write enable. */
    GWE = 1,
    /** GRF write select. */
    GWS0 = 2,
    GWS1 = 3,
    /** GRF read select. */
    GRS0 = 4,
    GRS1 = 5,
    /** Rotate right. */
    RR = 6,
    /** Rotate left. */
    RL = 7,
    /** Operation select. */
    OP0 = 8,
    OP1 = 9,
    /** Invert B-bus. */
    INB = 10,
    /** Enable carry. */
    EC = 11,
    /** Select auxiliary carry. */
    SA = 12,
    /** Flags output enable. */
    FEO = 13,
    /** Flags select mask. */
    FS0 = 14,
    FS1 = 15,
    FS2 = 16,
    FS3 = 17,
    FS4 = 18,
    FS5 = 19,
    FS6 = 20,
    FS7 = 21,

    /** Carry flag. */
    FS_C = FS0,
    /** Zero flag. */
    FS_Z = FS1,
    /** Sign flag. */
    FS_S = FS2,
    /** Overflow flag. */
    FS_V = FS3,
    /** Auxiliary carry flag. */
    FS_AC = FS4,
    /** Enable interrupt flag. */
    FS_E = FS5,
    /** Prefix flag. */
    FS_P = FS6,
    /** Interrupt flag. */
    FS_I = FS7,

    /** Flags write select. */
    FWS = 22,
    /** SRF write select. */
    SWS0 = 23,
    SWS1 = 24,
    SWS2 = 25,
    /** SRF write enable. */
    SWE = 26,
    /** SRF read select. */
    SRS0 = 27,
    SRS1 = 28,
    SRS2 = 29,
    /** SRF output enable. */
    SOE = 30,
    /** Increment SP. */
    ISP = 31,
    /** Increment IP. */
    IIP = 32,
    /** Decrement. */
    DEC = 33,
    /** Enable DS (data segment). */
    DS = 34,
    /** Enable CS (code segment). */
    CS = 35,
    /** Enable high part of address. */
    HE = 36,
    /** High part select. */
    HS0 = 37,
    HS1 = 38,
    /** Write data to external bus. */
    WR = 39,
    /** Conditional jump enable. */
    CND = 40,
    /** Read data from external bus. */
    RD = 41,
    /** Handle next opcode. */
    NXT = 42,
    INA = 43,
    /** Next micro-instruction pointer. */
    NMIP0 = 44,
    NMIP1 = 45,
    NMIP2 = 46,
    NMIP3 = 47,
    NMIP4 = 48,
    NMIP5 = 49,
    NMIP6 = 50,
    NMIP7 = 51,
    NMIP8 = 52,
    NMIP9 = 53,
    NMIP10 = 54,
    NMIP11 = 55,

    /** Next micro-instruction pointer for conditional jump. Bits are treated as regular control bits. */
    CND_NMIP0 = GOE,
    CND_NMIP1 = GWE,
    CND_NMIP2 = GWS0,
    CND_NMIP3 = GWS1,
    CND_NMIP4 = GRS0,
    CND_NMIP5 = GRS1,
    CND_NMIP6 = RR,
    CND_NMIP7 = RL,
    CND_NMIP8 = OP0,
    CND_NMIP9 = OP1,
    CND_NMIP10 = INB,
    CND_NMIP11 = EC
};

struct MicInstrBits {
    static constexpr int SIZE_BITS = 56;
    static constexpr int SIZE_BYTES = SIZE_BITS / 8;

    std::bitset<SIZE_BITS> bits;

    inline bool get_bit(Bit start) const { return bits.test(start); };
    void set_field(Bit start, size_t size, int value);
    int get_field(Bit start, size_t size) const;
    /** Поиск конфликтов. */
    std::vector<std::string> find_conflicts() const;
};

#endif