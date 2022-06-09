#ifndef DIAGN_H
#define DIAGN_H

#include <vector>
#include <string>
#include <exception>

struct Position {
    char *path;
    unsigned long line;

    Position(char *path, unsigned long line);
    Position();

    std::string to_str() const;
};

class Exception : std::exception {
public:
    const std::string &message;
    const Position *pos;
    Exception(const Position *pos, const std::string &message);
};

class Diagn {
public:
    static std::vector<std::string> errors, warnings;

    static void issue(bool isError, const Position *pos, const std::string &message);
    static void error(const Position *pos, const std::string &message);
    static void warning(const Position *pos, const std::string &message);

    static void report(std::ostream &stream);
};

#endif