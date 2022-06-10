#include "../../inc/devs.h"

Ram::Ram(size_t size) {
    bytes = new uint8_t[size];
}

Ram::~Ram() {
    delete[] bytes;
}

uint8_t Ram::read(addr_t address) {
    return bytes[address];
}

void Ram::write(addr_t address, uint8_t data) {
    bytes[address] = data;
}

bool Ram::has_irq() {
    /** ОЗУ не может запрашивать прерываний. */
    return false;
}
