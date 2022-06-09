#include "../inc/cursor.h"
#include "../inc/diagn.h"
#include <utility>

Cursor::Cursor(char *path, std::istream *stream)
        : c(0),
          stream(stream),
          is_eof(false),
          pos(path, 1) {}

Cursor::~Cursor() {
    delete stream;
}

char Cursor::eat_char() {
    int i = stream->get();
    if (i == EOF)
        is_eof = true;

    c = (char) i;
    return c;
}

bool Cursor::eat_if_eq(char _c) {
    if (this->c != _c)
        return false;

    eat_char();
    return true;
}

void Cursor::skip_blank() {
    while ((c == ' ' || c == '\r' || c == '\t') && !is_eof)
        eat_char();
}

void Cursor::skip_line() {
    while (c != '\n' && !is_eof)
        eat_char();
}

void Cursor::skip_comment() {
    if (c != '#')
        return;

    skip_line();
}

bool Cursor::handle_newline() {
    if (!eat_if_eq('\n'))
        return false;

    pos.line++;
    return true;
}

std::string Cursor::eat_ident() {
    std::string ident;

    while ((std::isalnum(c) || c == '_') && !is_eof) {
        ident.push_back(c);
        eat_char();
    }

    return ident;
}

std::string Cursor::eat_opnd() {
    std::string opnd;

    if (eat_if_eq('$'))
        opnd.push_back('$');

    opnd += eat_ident();
    return opnd;
}

std::string Cursor::eat_param() {
    if (c != '(')
        return "";

    std::string param;
    while (eat_char() != ')') {
        if (c == '\n' && !is_eof) {
            Diagn::error(&pos, "expected closing ')'");
            return "";
        }
        param.push_back(c);
    }

    eat_char(); // Eating closing ')'.

    return param;
}

Cursor *Cursor::from_file_path(char *path) {
    auto stream = new std::ifstream(path);
    if (!*stream)
        return nullptr;

    return new Cursor(path, stream);
}
