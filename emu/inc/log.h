#ifndef K8_18_LOG_H
#define K8_18_LOG_H

#include <string>
#include <sstream>
#include <iomanip>

std::string i2hex(uint32_t i, int digs_num);
inline std::string i2hex(uint32_t i) { return i2hex(i, 5); }

class Log {
public:
    static void report(const std::string &message);
};

#endif
