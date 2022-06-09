#ifndef GEN_H
#define GEN_H

#include <vector>
#include <cstdint>
#include <iostream>
#include "pass1.h"

/** Ошибка, возникшая при выделении адреса. */
class AllocationException : Exception {
public:
    AllocationException(const Position *pos, const std::string &message);;
};

class PassTwo {
    static constexpr uint16_t MC_SIZE = 4096;
    static constexpr uint16_t MP_START = 0;
    static constexpr uint16_t MP_END = MP_START + 255;
    static constexpr uint16_t PP_START = 0x8ff;
    static constexpr uint16_t PP_END = PP_START + 255;

    /** Карта адресов микрокода. */
    std::bitset<MC_SIZE> map;
    std::map<std::string, Label> (&labels)[4];
    std::vector<MicInstr> &mic_instrs;
    /** Маска, которая применяется к каждой микроинструкции с помощью XOR. */
    uint64_t mic_instr_mask;
    /** Выходной поток бинарного микрокода. */
    std::ostream &bin_stream;
    /** Выходной поток с информацией о метках. */
    std::ostream &labels_stream;
    uint8_t *bytes;

    void report_label(const std::string &name, const Label &label);
    /** Обработка меток, которые необходимо распределить на промежутке start...end. Возвращает адрес последней
     *  распределённой метки. */
    uint16_t alloc_labels(std::map<std::string, Label> &labels_map, uint16_t start, uint16_t end);
    /** Выделение адреса в промежутке start...end. Если в нём нет свободного места, выбрасывается
     *  AllocationException, иначе возвращается выделенный адрес. */
    uint16_t alloc(const Position *pos, uint16_t start, uint16_t end);
    /** Попытка занять адрес. Если адрес уже занят, то будет выброшено исключение. */
    void try_occupy(const Position *pos, uint16_t address);
    /** Поиск метки с именем name. Если она найдена, то address изменяется на адрес метки, возвращается true, иначе
     *  false и выдаётся сообщение об ошибке на позиции pos. */
    bool find_label_address(const Position *pos, const std::string &name, uint16_t &address);

public:
    PassTwo(std::map<std::string, Label> (&labels)[4], std::vector<MicInstr> &mic_instrs,
            uint64_t mic_instr_mask, std::ostream &bin, std::ostream &labels_stream);
    ~PassTwo();

    void exec();
};

#endif