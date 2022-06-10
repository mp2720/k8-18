#ifndef SCANNER_H
#define SCANNER_H

#include "diagn.h"
#include <fstream>
#include <string>
#include <cstdint>

class Cursor {
    std::istream *stream;

public:
    char c;
    bool is_eof;
    Position pos;

    Cursor(char *path, std::istream *stream);
    ~Cursor();

    char eat_char();
    bool eat_if_eq(char _c);
    void skip_blank();
    void skip_line();
    void skip_comment();
    bool handle_newline();
    std::string eat_ident();
    std::string eat_opnd();
    std::string eat_param();

    static Cursor *from_file_path(char *path);
};

#endif