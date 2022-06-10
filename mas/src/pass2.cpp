#include "../inc/pass2.h"
#include <cstring>
#include <iomanip>
#include <sstream>

AllocationException::AllocationException(const Position *pos, const std::string &message) :
        Exception(pos, message) {}

std::string i2hex(uint16_t i) {
    std::stringstream stream;
    stream << "0x"
           << std::setfill('0') << std::setw(3)
           << std::hex << i;
    return stream.str();
}

PassTwo::PassTwo(std::map<std::string, Label> (&labels)[4], std::vector<MicInstr> &mic_instrs,
                 uint64_t mic_instr_mask, std::ostream &bin_stream, std::ostream &labels_stream) :
        labels(labels),
        mic_instrs(mic_instrs),
        mic_instr_mask(mic_instr_mask),
        bin_stream(bin_stream),
        labels_stream(labels_stream) {
    bytes = new uint8_t[BYTES_SIZE];
    memset((void *) bytes, 0, BYTES_SIZE * sizeof(*bytes));
}

PassTwo::~PassTwo() {
    delete bytes;
}

void PassTwo::report_label(const std::string &name, const Label &label) {
    labels_stream << name << ": " << i2hex(label.address) << '\n';
}

uint16_t PassTwo::alloc_labels(std::map<std::string, Label> &labels_map, uint16_t start, uint16_t end) {
    uint16_t cur = start;
    for (auto &pair: labels_map) {
        auto &label = pair.second;
        label.address = alloc(&label.pos, start, end);
        report_label(pair.first, label);
        cur = label.address + 1;
    }

    return cur;
}

uint16_t PassTwo::alloc(const Position *pos, uint16_t start, uint16_t end) {
    for (uint16_t cur = start; cur <= end; cur++) {
        if (!map.test(cur)) {
            map.set(cur);
            return cur;
        }
    }

    throw AllocationException(pos, "no space left");
}

void PassTwo::try_occupy(const Position *pos, uint16_t address) {
    if (map.test(address))
        throw AllocationException(pos, "address is already occupied");
    else
        map.set(address);
}

bool PassTwo::find_label_address(const Position *pos, const std::string &name, uint16_t &address) {
    for (auto &labels_map: labels)
        for (auto &pair: labels_map)
            if (pair.first == name) {
                address = pair.second.address;
                return true;
            }

    Diagn::error(pos, "unknown label '" + name + "'");
    return false;
}

void PassTwo::proc_prev_mic_instr(MicInstr *prev, uint16_t cur_address, uint16_t prev_address) {
    if (prev == nullptr)
        return;

    // Если NMIP не был указан, значит NMIP предыдущей микроинструкции - адрес текущей.
    if (prev->nmip.type == Nmip::TYPE_NONE) {
        if (cur_address == prev_address)
            prev->nmip = Nmip(0); // Если текущий адрес равен предыдущему, то это последняя микроинструкция.
        else
            prev->nmip = Nmip(cur_address);
    }

    prev->bits.set_field(NMIP0, 12, prev->nmip.address);

    if (prev->bits.get_bit(CND))
        prev->bits.set_field(CND_NMIP0, 12, prev->cnd_nmip.address);

    // Запись байтов микроинструкции в массив.
    for (int i = 0, bit = 48; i < 7; i++, bit -= 8)
        bytes[prev_address * MicInstrBits::SIZE_BYTES + i] = prev->bits.get_field(Bit(bit), 8);
}

void PassTwo::exec() {
    // @instr(<cur_address>).
    for (auto &pair: labels[PAGE_WITH_ADDRESS]) {
        auto &label = pair.second;
        try_occupy(&label.pos, label.address);
        report_label(pair.first, label);
    }

    // @instr.
    uint16_t last = alloc_labels(labels[PAGE_MAIN], MP_START, MP_END);
    // @instr(pref).
    alloc_labels(labels[PAGE_PREFIX], PP_START, PP_END);
    // Остальные метки.
    last = alloc_labels(labels[PAGE_OTHER], last, MC_SIZE - 1);

    MicInstr *prev = nullptr;
    uint16_t prev_address = 0;
    uint16_t cur_address;

    for (auto &mi: mic_instrs) {
        if (mi.label.empty())
            cur_address = alloc(&mi.pos, last, MC_SIZE - 1);
        else if (!find_label_address(&mi.pos, mi.label, cur_address))
            continue; // Если метка не найдена, значит адрес не установлен и дальнейшие действия не требуются.

        proc_prev_mic_instr(prev, cur_address, prev_address);
        prev = &mi;
        prev_address = cur_address;
    }

    // Последняя инструкция тоже должна быть обработана.
    proc_prev_mic_instr(prev, cur_address, prev_address);

    bin_stream.write((char *) bytes, std::streamsize(BYTES_SIZE * sizeof(*bytes)));
}
