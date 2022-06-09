#include "../inc/diagn.h"
#include <ostream>

std::vector<std::string> Diagn::errors, Diagn::warnings;

Position::Position(char *path, unsigned long line) :
        path(path),
        line(line) {}

Position::Position() :
        path(nullptr),
        line(1) {}

std::string Position::to_str() const {
    return std::string(path) + ":" + std::to_string(line);
}

Exception::Exception(const Position *pos, const std::string &message) :
        pos(pos),
        message(message) {}

void Diagn::issue(bool isError, const Position *pos, const std::string &message) {
    std::string str = isError ? "ERROR" : "WARNING";

    if (pos != nullptr)
        str += " " + pos->to_str();

    str += ": " + message;

    if (isError)
        errors.push_back(str);
    else
        warnings.push_back(str);
}

void Diagn::error(const Position *pos, const std::string &message) {
    issue(true, pos, message);
}

void Diagn::warning(const Position *pos, const std::string &message) {
    issue(false, pos, message);
}

void Diagn::report(std::ostream &stream) {
    for (auto &error: errors)
        stream << error << '\n';

    for (auto &warning: warnings)
        stream << warning << '\n';
}
