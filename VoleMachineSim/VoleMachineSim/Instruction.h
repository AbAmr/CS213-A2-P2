#pragma once

#include <cstdint>
#include "Memory.h"
#include "RegisterFile.h"

enum class ExecutionStatus {
    CONTINUE,
    HALTED,
    ERROR
};

class Instruction {
private:
    uint16_t rawInstruction;

public:
    Instruction(uint16_t raw = 0);

    void setInstruction(uint16_t raw);

    uint8_t getOpcode() const;
    uint8_t getR() const;
    uint8_t getX() const;
    uint8_t getY() const;
    uint8_t getAddress() const;
};