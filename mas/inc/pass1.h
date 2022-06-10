#ifndef PASS_ONE_H
#define PASS_ONE_H

#include <string>
#include <cstdint>
#include <list>
#include <optional>
#include <map>

#include "diagn.h"
#include "cursor.h"
#include "mic_instr_bits.h"

struct Label {
    uint16_t address;
    Position pos;

    inline Label() : address(0) {};
};

struct Nmip {
    uint16_t address;
    std::string label;

    enum {
        TYPE_ADDRESS,
        TYPE_LABEL,
        TYPE_NONE
    } type;

    Nmip();
    explicit Nmip(uint16_t address);
    explicit Nmip(std::string label);
};

struct MicInstr {
    MicInstrBits bits;
    std::string label;
    Nmip nmip;
    Nmip cnd_nmip;
    Position pos;
};

enum State {
    WAITING_ANY,
    WAITING_LABEL,
    WAITING_MIC_INSTR
};

enum Page {
    PAGE_WITH_ADDRESS = 0,
    PAGE_MAIN = 1,
    PAGE_PREFIX = 2,
    PAGE_OTHER = 3
};

class PassOne {
    static constexpr int MAX_INCLUDES = 100;
    /** For handling recursive inclusion. */
    static int includes_count;

    /** Пути к включенным файлам. */
    static std::vector<std::string> paths;

    Cursor *cursor;
    Label cur_label;
    Page cur_label_page;
    MicInstr cur_mic_instr;
    State state;

    /** Reports error diagnostics message with `cursor`s position. */
    void error(const std::string &message);
    /** Same as `error`, but with warning message. */
    void warning(const std::string &message);
    /** Same as `error`, but with 'expected' word. */
    void expected(const std::string &str);
    /** Checks current state, compares with `expected_state` and reports error if state is unexpected. */
    void check_state(State expected_state);

    /** Try add label or report error with diagnostics message. */
    void try_add_label(const std::string &ident, Label &label);
    /** Try parse hexadecimal number. Number will be stored in `value` in case of success. */
    void parse_hex(const std::string &str, int &value);
    void set_bit(Bit bit, const std::string &name);
    void set_field(Bit start, int size, int value, const std::string &name);

    void proc_jump_opnd(const std::string &opnd, Nmip &nmip);
    void proc_mem_opnd(const std::string &opnd1, const std::string &opnd2);
    void proc_reg8_opnd(bool is_dest, const std::string &opnd);

    void proc_dir(const std::string &dir);
    void proc_label(const std::string &ident);
    void proc_ctrl_signal(const std::string &ctrl_signal);
    void proc_mnemonic(const std::string &str);
    void proc_mic_instr(bool is_mnemonic, const std::string &first_tok);

public:
    std::map<std::string, Label> labels[4];
    std::vector<MicInstr> mic_instrs;

    explicit PassOne(Cursor *cursor);
    PassOne();
    ~PassOne();

    /** Execute stage. */
    void exec();
};

#endif
