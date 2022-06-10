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

class Exception : public std::exception {
    std::string what_message;

public:
    Exception(const Position *pos, const std::string &message);

    const char *what() const noexcept override;
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