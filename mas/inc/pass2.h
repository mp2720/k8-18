#ifndef GEN_H
#define GEN_H

#include <vector>
#include <cstdint>
#include <iostream>
#include "pass1.h"

/** Ошибка, возникшая при выделении адреса. */
class AllocationException : public Exception {
public:
    AllocationException(const Position *pos, const std::string &message);
};

class PassTwo {
public:
    static constexpr uint16_t MC_SIZE = 4096;

private:
    static constexpr uint16_t MP_START = 0;
    static constexpr uint16_t MP_END = MP_START + 255;
    static constexpr uint16_t PP_START = 0x800;
    static constexpr uint16_t PP_END = PP_START + 255;

    /** Карта адресов микрокода. */
    std::bitset<MC_SIZE> map;
    std::map<std::string, Label> (&labels)[4];
    std::vector<MicInstr> &mic_instrs;
    /** Выходной поток бинарного микрокода. */
    std::ostream &bin;
    /** Выходной поток с информацией о метках. */
    std::ostream &info;
    static constexpr size_t BYTES_SIZE = MC_SIZE * MicInstrBits::SIZE_BYTES;
    uint8_t *bytes;

    void report_label(const std::string &name, const Label &label);
    /** Занять адрес и увеличить occupied. */
    void occupy(uint16_t address);
    /** Попытка занять адрес. Если адрес уже занят, то будет выброшено исключение. */
    void try_occupy(const Position *pos, uint16_t address);
    /** Выделение адреса в промежутке start...end. Если в нём нет свободного места, выбрасывается
     *  AllocationException, иначе возвращается выделенный адрес. */
    uint16_t alloc(const Position *pos, uint16_t start, uint16_t end);
    /** Обработка меток, которые необходимо распределить на промежутке start...end. Возвращает адрес последней
     *  распределённой метки. */
    uint16_t alloc_labels(std::map<std::string, Label> &labels_map, uint16_t start, uint16_t end);
    /** Поиск метки с именем name. Если она найдена, то address изменяется на адрес метки, возвращается true, иначе
     *  false и выдаётся сообщение об ошибке на позиции pos. */
    bool find_label_address(const Position *pos, const std::string &name, uint16_t &address);
    void proc_prev_mic_instr(MicInstr *prev, uint16_t cur_address, uint16_t prev_address);
    /** Вывести информацию о заполнении микрокода в info. */
    void report_map();

public:
    /** Количество занятых адресов. */
    uint16_t occupied;

    PassTwo(std::map<std::string, Label> (&labels)[4], std::vector<MicInstr> &mic_instrs,
            uint64_t mic_instr_mask, std::ostream &bin, std::ostream &info);
    ~PassTwo();

    void exec();

};

#endif