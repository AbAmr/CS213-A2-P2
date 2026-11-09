#pragma once

#include <cstdint>

#include "ExecutionError.h"
#include "Instruction.h"

class Memory;
class RegisterFile;
class ALU;

namespace VoleError {
    constexpr int PC_OUT_OF_RANGE     = 1;
    constexpr int INVALID_INSTRUCTION = 2;
    constexpr int INVALID_REGISTER    = 3;
    constexpr int INVALID_ADDRESS     = 4;
    constexpr int INVALID_JUMP        = 5;
    constexpr int STEP_LIMIT          = 6;
}

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
