#ifndef K8_18_BUS_H
#define K8_18_BUS_H

#include <vector>
#include <utility>
#include <cstdint>

typedef uint32_t addr_t;

class Bus;

class Device {
public:
    virtual void write(addr_t address, uint8_t data) = 0;
    virtual uint8_t read(addr_t address) = 0;
    virtual bool has_irq() = 0;
};

class Bus {
    /** Значение, которое выдаётся, если операция чтения завершилась с ошибкой (это значение может быть выдано
     * конкретным устройством и при нормальной работе). */
    static constexpr uint8_t FAIL_VALUE = 0xEB;

    struct MemMapEntry {
        addr_t start;
        addr_t end;
        Device &device;
    };

    std::vector<MemMapEntry> mem_map;

    Device *find_device(addr_t address);

public:
    void write(addr_t address, uint8_t data);
    uint8_t read(addr_t address);
    bool has_irq();
};

#endif
