#include "../inc/pass1.h"

#include <utility>

Nmip::Nmip() :
        type(TYPE_NONE),
        address(0) {}

Nmip::Nmip(uint16_t address) :
        type(TYPE_ADDRESS),
        address(address) {}

Nmip::Nmip(std::string label) :
        type(TYPE_LABEL),
        address(0),
        label(std::move(label)) {}

int PassOne::includes_count = 0;
std::vector<std::string> PassOne::paths;

PassOne::PassOne(Cursor *cursor)
        : cursor(cursor),
          state(WAITING_ANY),
          cur_label_page(PAGE_OTHER) {}

PassOne::~PassOne() {
    delete cursor;
}

void PassOne::error(const std::string &message) {
    Diagn::error(&cursor->pos, message);
}

void PassOne::warning(const std::string &message) {
    Diagn::warning(&cursor->pos, message);
}

void PassOne::expected(const std::string &str) {
    error("expected " + str);
}

void PassOne::check_state(State expected_state) {
    if (state != expected_state) {
        switch (state) {
            case WAITING_LABEL:
                expected("label");
                break;

            case WAITING_MIC_INSTR:
                expected("micro-instruction");
                break;

            case WAITING_ANY:
                // Not an error.
                break;
        }
    }
}

void PassOne::try_add_label(const std::string &ident, Label &label) {
    for (auto &map: labels) {
        if (map.find(ident) != map.end()) {
            Diagn::error(&label.pos, "label '" + ident + "' is already defined at "
                                     + map[ident].pos.to_str());
            return;
        }
    }

    labels[cur_label_page].insert(std::make_pair(ident, cur_label));
}

void PassOne::parse_hex(const std::string &str, int &value) {
    try {
        size_t idx;
        int num = std::stoi(str, &idx, 16);
        if (idx != str.size()) {
            // После цифр есть неразобранные символы, не являющиеся частью числа.
            error("invalid symbols at the end of number");
            return;
        }

        value = num;
    } catch (std::invalid_argument &e) {
        expected("hexadecimal number or 'pref'");
    } catch (std::out_of_range &e) {
        // За пределами типа int.
        error("number out of range");
    }
}

void PassOne::set_bit(Bit bit, const std::string &name) {
    if (cur_mic_instr.bits.bits.test(bit))
        warning("bit '" + name + "' was set multiple times");
    else
        cur_mic_instr.bits.bits.set(bit);
}

void PassOne::set_field(Bit start, int size, int value, const std::string &name) {
    if (cur_mic_instr.bits.get_field(start, size) != 0)
        warning("field '" + name + "' was set multiple times");
    else if (value >= 1 << size)
        error("value is too big for " + std::to_string(size) + " bit field");
    else
        cur_mic_instr.bits.set_field(start, size, value);
}

void PassOne::proc_jump_opnd(const std::string &opnd, Nmip &nmip) {
    if (opnd[0] == '$') {
        // Дано шестнадцатеричное значение.
        int address;
        parse_hex(opnd.substr(1), address);

        nmip = Nmip(address);
    } else
        // Дана метка.
        nmip = Nmip(opnd);
}

void PassOne::proc_mem_opnd(const std::string &opnd1, const std::string &opnd2) {
    if (opnd1 == "cs")
        set_bit(CS, "cs");
    else if (opnd1 == "ds")
        set_bit(DS, "ds");
    else if (opnd1 != "0") {
        expected("data, code or 0 segment as wr/rd first operand");
        return;
    }

    if (opnd2 == "0")
        return;

    // Если операнд не равен "0", то нужно выставить разрешение на формирование старшей части адреса.
    set_bit(HE, "he");

    // Индекс соответствует значению HS.
    std::string high_regs[4] = {"iph", "sph", "tmph", "y"};
    for (int i = 0; i < 4; i++) {
        if (high_regs[i] == opnd2) {
            // В случае, если в качестве операнда используется старший регистр, младшая часть адреса формируется на шине
            // C.
            set_field(HS0, 2, i, "hs");
            return;
        }
    }

    if (opnd2 == "p" || opnd2 == "yx") {  // Один и тот же регистр с двумя именами.
        set_field(HS0, 2, 3, "hs");

        // Считать X.
        set_bit(GOE, "goe");
        set_field(GRS0, 2, 0, "grs");
    } else {
        // Номер регистра для SRS.
        int low_reg;
        if (opnd2 == "ip")
            low_reg = 0;
        else if (opnd2 == "sp")
            low_reg = 2;
        else if (opnd2 == "tmp")
            low_reg = 4;
        else {
            expected("8-bit registers: IPH, SPH, TMPH, Y, 16-bits registers: IP, SP, TMP, P/YX or 0 as wr/rd second "
                     "operand");
            return;
        }

        set_field(SRS0, 3, low_reg, "srs");
        set_bit(SOE, "soe");
    }
}

void PassOne::proc_reg8_opnd(bool is_dest, const std::string &opnd) {
    if (opnd == "0")
        return;

    std::string gregs[4] = {"b", "a", "y", "x"};
    std::string sregs[8] = {"ipl", "iph", "spl", "sph", "tmpl", "tmph", "md", "ss"};

    // Поиск в GRF.
    for (int i = 0; i < 4; i++) {
        if (opnd == gregs[i]) {
            if (is_dest) {
                set_bit(GWE, "gwe");
                set_field(GWS0, 2, i, "gws");
            } else {
                set_bit(GOE, "goe");
                set_field(GRS0, 2, i, "grs");
            }
            return;
        }
    }

    // Поиск в SRF.
    for (int i = 0; i < 8; i++) {
        if (opnd == sregs[i]) {
            if (is_dest) {
                set_bit(SWE, "swe");
                set_field(SWS0, 3, i, "sws");
            } else {
                set_bit(SOE, "soe");
                set_field(SRS0, 3, i, "srs");
            }
            return;
        }
    }

    expected("B, A, X, Y, IPL, IPH, SPL, SPH, TMPL, TMPH, MD, SS register");
}

void PassOne::proc_dir(const std::string &dir) {
    check_state(WAITING_ANY);

    auto param = cursor->eat_param();

    if (dir == "instr") {
        state = WAITING_LABEL;

        if (param.empty())
            cur_label_page = PAGE_MAIN;
        else if (param == "pref")
            cur_label_page = PAGE_PREFIX;
        else {
            // Параметром является адрес.
            int num = 0;
            parse_hex(param, num);
            cur_label.address = (uint16_t) num;
            cur_label_page = PAGE_WITH_ADDRESS;
        }
    } else if (dir == "include") {
        if (param.empty()) {
            expected("path in parenthesis after @include");
            return;
        }

        // Проверка на рекурсивность.
        if (includes_count >= MAX_INCLUDES) {
            error("too many includes (maximum is " + std::to_string(MAX_INCLUDES)
                  + "). Check for recursive @include()");
            return;
        }

        paths.push_back(param);

        auto *include_cursor = Cursor::from_file_path(paths.back().data());
        if (include_cursor == nullptr) {
            error("cannot open file");
            return;
        }

        auto po = PassOne(include_cursor);
        includes_count++;
        po.exec();
        includes_count--;

        // Включение меток.
        for (auto &map: po.labels)
            for (auto &ident_label: map)
                try_add_label(ident_label.first, ident_label.second);

        // Включение микроинструкций.
        mic_instrs.insert(mic_instrs.end(), po.mic_instrs.begin(), po.mic_instrs.end());
    } else
        expected("@instr or @include directive");
}

void PassOne::proc_label(const std::string &ident) {
    check_state(WAITING_LABEL);

    cur_label.pos = cursor->pos;
    try_add_label(ident, cur_label);

    // Следующая непустая строка должна содержать микроинструкцию.
    cur_mic_instr.label = ident;
    state = WAITING_MIC_INSTR;

    // Значение по-умолчанию.
    cur_label_page = PAGE_OTHER;
}

void PassOne::proc_ctrl_signal(const std::string &ctrl_signal) {
    const std::string bits[] = {
            "goe", "gwe", "gws0", "gws1", "grs0", "grs1", "rr", "rl", "op0",
            "op1", "inv", "ec", "sa", "feo", "fs0", "fs1", "fs2", "fs3",
            "fs4", "fs5", "fs6", "fs7", "fws", "sws0", "sws1", "sws2", "swe",
            "srs0", "srs1", "srs2", "soe", "isp", "iip", "dec", "ds", "cs",
            "he", "hs0", "hs1", "wr", "cnd", "rd", "nxt", "reserved"
    };

    // Key is name of field, value is a pair of first bit and size (in bits).
    std::map<const std::string, std::pair<Bit, int>> fields = {
            {"gws", {GWS0, 2}},
            {"grs", {GRS0, 2}},
            {"op",  {OP0,  2}},
            {"fs",  {FS0,  8}},
            {"sws", {SWS0, 3}},
            {"srs", {SRS0, 3}},
            {"hs",  {HS0,  2}}
    };

    int bit = 0;
    for (; bit < 44; bit++) {
        if (bits[bit] == ctrl_signal) {
            // Handling control bit.
            set_bit(Bit(bit), ctrl_signal);
            return;
        }
    }

    // Else handling control field.
    auto field = fields.find(ctrl_signal);
    if (field == fields.end()) {
        error("unknown control bit or field '" + ctrl_signal + "'");
        return;
    }

    auto pair = field->second;
    auto param = cursor->eat_param();

    if (param.empty()) {
        expected("parameter in parenthesis after '" + ctrl_signal + "' field");
        return;
    }

    int num = 0;
    parse_hex(param, num);
    set_field(pair.first, pair.second, num, ctrl_signal);
}


void PassOne::proc_mnemonic(const std::string &str) {
    enum Mnemonic {
        MN_ADD,
        MN_AND,
        MN_OR,
        MN_XOR,

        MN_RD,
        MN_WR,
        MN_MOV,

        MN_DEC,
        MN_INC,
        MN_FS,
        MN_JCND,
        MN_JMP,

        MN_NOP,
        MN_LDF,
        MN_STF,
        MN_INV
    };
    std::map<const std::string, std::pair<int, Mnemonic>> mnemonics = {
            {"add",  {3, MN_ADD}},
            {"or",   {3, MN_OR}},
            {"and",  {3, MN_AND}},
            {"xor",  {3, MN_XOR}},

            {"rd",   {2, MN_RD}},
            {"wr",   {2, MN_WR}},
            {"mov",  {2, MN_MOV}},

            {"dec",  {1, MN_DEC}},
            {"inc",  {1, MN_INC}},
            {"fs",   {1, MN_FS}},
            {"jcnd", {1, MN_JCND}},
            {"jmp",  {1, MN_JMP}},

            {"nop",  {0, MN_NOP}},
            {"ldf",  {0, MN_LDF}},
            {"stf",  {0, MN_STF}},
            {"inv",  {0, MN_INV}}
    };

    std::string opnds[3];
    int opnds_count = 0;

    do {
        cursor->skip_blank();

        opnds[opnds_count] = cursor->eat_opnd();
        if (opnds_count >= 3) {
            error("too many opnds provided");
            return;
        }

        if (opnds[opnds_count].empty()) {
            if (opnds_count != 0) {
                error("expected operand after ','");
                return;
            }
            break;
        }

        cursor->skip_blank();
        opnds_count++;
    } while (cursor->eat_if_eq(','));

    auto iterator = mnemonics.find(str);
    if (iterator == mnemonics.end()) {
        error("unknown mnemonic '" + str + "'");
        return;
    }

    Mnemonic mnemonic = iterator->second.second;
    int expected_operands_count = iterator->second.first;

    if (opnds_count != expected_operands_count) {
        error("'" + str + "' handles " + std::to_string(expected_operands_count) + " opnds, but "
              + std::to_string(opnds_count) + " where given");
        return;
    }

    switch (mnemonic) {
        // 0 операндов:
        case MN_NOP:
            // No operation.
            break;
        case MN_LDF:
            // Выгрузить флаги на шину C.
            set_bit(FEO, "feo");
            break;
        case MN_STF:
            // Загрузить флаги с шины С.
            set_bit(FWS, "fws");
            break;
        case MN_INV:
            // Инвертировать шину С.
            set_bit(INV, "inv");
            break;


            // 1 операнд:
        case MN_DEC:
            set_bit(DEC, "dec");
            // Для 'dec' нужны те же биты, что и для 'inc', за исключением бита DEC.
        case MN_INC:
            if (opnds[0] == "ip")
                set_bit(IIP, "iip");
            else if (opnds[0] == "sp")
                set_bit(ISP, "isp");
            else
                expected("IP or SP for INC");
            break;
        case MN_FS:
            for (char c: opnds[0]) {
                std::map<char, Bit> flags = {
                        {'c', FS_C},
                        {'z', FS_Z},
                        {'s', FS_S},
                        {'v', FS_V},
                        {'a', FS_AC},
                        {'e', FS_E},
                        {'p', FS_P},
                        {'i', FS_I}
                };

                auto iter = flags.find(c);
                if (iter == flags.end())
                    error("unknown flags mask character '" + std::string(1, c) + "'");
                else
                    set_bit(iter->second, std::string("fs_") + std::string(1, c));
            }
            break;
        case MN_JCND:
            // Jump conditionally to label or address.
            set_bit(CND, "cnd");
            proc_jump_opnd(opnds[0], cur_mic_instr.cnd_nmip);
            break;
        case MN_JMP:
            // Jump unconditionally to label or address.
            proc_jump_opnd(opnds[0], cur_mic_instr.nmip);
            break;


            // 2 операнда:
        case MN_WR:
            proc_mem_opnd(opnds[0], opnds[1]);
            set_bit(WR, "wr");
            break;
        case MN_RD:
            proc_mem_opnd(opnds[0], opnds[1]);
            set_bit(RD, "rd");
            break;
        case MN_MOV:
            proc_reg8_opnd(true, opnds[0]);
            proc_reg8_opnd(false, opnds[1]);
            break;


            // 3 операнда:
        default:
            set_field(OP0, 2, mnemonic - MN_ADD, "OP");
            proc_reg8_opnd(true, opnds[0]);
            proc_reg8_opnd(false, opnds[1]);
            proc_reg8_opnd(false, opnds[2]);
    }
}

void PassOne::proc_mic_instr(bool is_mnemonic, const std::string &first_tok) {
    check_state(WAITING_MIC_INSTR);

    std::string tok = first_tok;
    while (true) {
        if (tok.empty()) {
            if (!is_mnemonic) {
                // Possible only if '%' were given.
                expected("control signal was expected after '%'");
                return;
            } else {
                warning("redundant semicolon");
                break;
            }
        }

        if (is_mnemonic)
            proc_mnemonic(tok);
        else
            proc_ctrl_signal(tok);

        cursor->skip_blank();
        if (!cursor->eat_if_eq(';'))
            break;

        cursor->skip_blank();
        if (cursor->eat_if_eq('%'))
            is_mnemonic = false;
        else
            is_mnemonic = true;

        tok = cursor->eat_ident();
    }

    auto conflicts = cur_mic_instr.bits.find_conflicts();
    if (!conflicts.empty()) {
        // Building error message.
        std::string msg = "conflicts found for micro-instruction bits: ";
        for (auto &conflict: conflicts) {
            msg += conflict;
            if (conflict != conflicts.back())
                msg += ", ";
        }
        msg += ". Check CPU and micro-assembler documentation for more info";
        error(msg);
    }

    mic_instrs.push_back(cur_mic_instr);
    cur_mic_instr = MicInstr();

    state = WAITING_ANY;
}

void PassOne::exec() {
    cursor->eat_char(); // Прочитать первый символ.

    while (true) {
        cursor->skip_blank();

        if (cursor->eat_if_eq('@'))
            proc_dir(cursor->eat_ident());
        else if (cursor->eat_if_eq('%'))
            proc_mic_instr(false, cursor->eat_ident());
        else {
            auto ident = cursor->eat_ident();
            if (!ident.empty()) {
                if (cursor->eat_if_eq(':'))
                    proc_label(ident);
                else
                    proc_mic_instr(true, ident);
            }
        }

        cursor->skip_blank();
        cursor->skip_comment();

        if (cursor->is_eof)
            break;
        else if (!cursor->handle_newline()) {
            error("invalid syntax at '" + std::string(1, cursor->c) + "'");
            cursor->skip_line();
        }
    }
}
PassOne::PassOne() :
        PassOne(nullptr) {}
