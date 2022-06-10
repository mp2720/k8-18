#include "../inc/log.h"

std::string i2hex(uint32_t i, int digs_num) {
    std::stringstream stream;
    stream << "0x"
           << std::setfill('0') << std::setw(digs_num)
           << std::hex << i;
    return stream.str();
}

void Log::report(const std::string &message) {

}
