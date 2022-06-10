#ifndef K8_18_DEVICES_H
#define K8_18_DEVICES_H

#include <cstdint>
#include <cstddef>
#include "bus.h"

class Ram : public Device {
    uint8_t *bytes;

public:
    explicit Ram(size_t size);
    ~Ram();

    uint8_t read(addr_t address) override;
    void write(addr_t address, uint8_t data) override;
    bool has_irq() override;
};

#endif
