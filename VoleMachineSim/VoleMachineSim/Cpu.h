#pragma once

#include <cstdint>

#include "ALU.h"
#include "ControlUnit.h"
#include "ExecutionError.h"
#include "ExecutionStatus.h"
#include "RegisterFile.h"

class Memory;

inline const char* statusName(ExecutionStatus status) {
    switch (status) {
        case ExecutionStatus::CONTINUE: return "CONTINUE";
        case ExecutionStatus::HALTED:   return "HALTED";
        case ExecutionStatus::ERROR:    return "ERROR";
    }
    return "UNKNOWN";
}

class CPU {
public:
    CPU();

    ExecutionStatus step(Memory& memory);
    ExecutionStatus run(Memory& memory);
    void reset();

    bool            isHalted() const;
    ExecutionStatus getStatus() const;
    ExecutionError  getError() const;
    void            dumpState();

    uint8_t  getRegister(uint8_t index) { return registerFile.read(index); }
    uint8_t  getPC() const              { return controlUnit.getPC(); }
    uint16_t getIR() const              { return controlUnit.getIR(); }

    bool setProgramCounter(uint8_t address);

private:
    ControlUnit     controlUnit;
    RegisterFile    registerFile;
    ALU             alu;
    ExecutionStatus status;
    ExecutionError  error;

    static constexpr unsigned long MAX_STEPS = 1000000UL;
};
