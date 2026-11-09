#pragma once

#include <cstdint>
#include <string>
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

    // read-only accessors (used by the GUI)
    uint8_t         readMemory(uint8_t address) { return memory.read(address); }
    uint8_t         getRegister(uint8_t index)  { return cpu.getRegister(index); }
    uint8_t         getPC() const               { return cpu.getPC(); }
    uint16_t        getIR() const               { return cpu.getIR(); }
    ExecutionStatus getStatus() const           { return cpu.getStatus(); }
    std::string     getErrorText() const        { ExecutionError e = cpu.getError(); return e.getError(); }

private:
    Memory memory;
    CPU    cpu;
};