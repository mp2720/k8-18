#include "../inc/bus.h"
#include "../inc/log.h"

Device *Bus::find_device(addr_t address) {
    for (auto &entry: mem_map)
        if (entry.start <= address && address <= entry.end)
            return &entry.device;

    return nullptr;
}

void Bus::write(addr_t address, uint8_t data) {
    auto *device = find_device(address);
    if (device == nullptr)
        Log::report("[BUS::wr] address " + i2hex(address) + " is not writeable (VALUE: "
                    + i2hex(data, 2) + ")");
    else
        device->write(address, data);
}

uint8_t Bus::read(addr_t address) {
    auto *device = find_device(address);
    if (device == nullptr) {
        Log::report("[BUS::rd] address " + i2hex(address) + " is not readable");
        return FAIL_VALUE;
    } else
        return device->read(address);
}

bool Bus::has_irq() {
    for (auto &entry: mem_map) {
        if (entry.device.has_irq())
            return true;
    }

    return false;
}
