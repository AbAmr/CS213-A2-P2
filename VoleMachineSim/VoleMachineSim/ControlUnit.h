#pragma once

#include <cstdint>

#include "ExecutionError.h"
#include "Instruction.h"

class Memory;
class RegisterFile;
class ALU;

class ControlUnit {
public:
    ControlUnit();

    Instruction fetch(Memory& memory, ExecutionError& error);

    bool execute(Instruction instruction, RegisterFile& regFile, ALU& alu,
                 Memory& memory, ExecutionError& error);

    bool isHalted() const;
    void setHalted(bool halted);
    void reset();

    uint8_t  getPC() const;
    uint16_t getIR() const;
    bool     setPCAddress(uint8_t address);

private:
    uint8_t  pc;
    uint16_t ir;
    bool     halted;
};
