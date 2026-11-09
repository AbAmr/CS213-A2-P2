#pragma once

#include <cstdint>
#include <vector>

#include "CPU.h"
#include "Memory.h"

class VoleMachine {
public:
    VoleMachine();

    bool loadProgram(const std::vector<uint8_t>& program, uint8_t startAddress);

    ExecutionStatus step();
    ExecutionStatus run();
    void reset();
    void displayStatus();
    void dumpMemory(uint8_t startAddress, uint16_t count);

private:
    Memory memory;
    CPU    cpu;
};
