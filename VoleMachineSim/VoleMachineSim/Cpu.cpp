#include "CPU.h"

#include <iostream>

#include "DisplayOutput.h"
#include "Memory.h"

CPU::CPU() : status(ExecutionStatus::CONTINUE) {}

ExecutionStatus CPU::step(Memory& memory) {
    if (status == ExecutionStatus::ERROR)
        return status;

    if (controlUnit.isHalted()) {
        status = ExecutionStatus::HALTED;
        return status;
    }

    Instruction instruction = controlUnit.fetch(memory, error);
    if (error.hasError()) {
        status = ExecutionStatus::ERROR;
        return status;
    }

    if (!controlUnit.execute(instruction, registerFile, alu, memory, error) || error.hasError()) {
        status = ExecutionStatus::ERROR;
        return status;
    }

    status = controlUnit.isHalted() ? ExecutionStatus::HALTED : ExecutionStatus::CONTINUE;
    return status;
}

ExecutionStatus CPU::run(Memory& memory) {
    unsigned long executed = 0;
    while (status == ExecutionStatus::CONTINUE) {
        if (executed++ >= MAX_STEPS) {
            error.setError(VoleError::STEP_LIMIT,
                           "Stopped after " + std::to_string(MAX_STEPS) +
                           " instructions (possible infinite loop)");
            status = ExecutionStatus::ERROR;
            break;
        }
        step(memory);
    }
    return status;
}

void CPU::reset() {
    controlUnit.reset();
    registerFile.reset();
    error.clear();
    status = ExecutionStatus::CONTINUE;
}

bool CPU::isHalted() const                { return controlUnit.isHalted(); }
ExecutionStatus CPU::getStatus() const    { return status; }
ExecutionError CPU::getError() const      { return error; }
bool CPU::setProgramCounter(uint8_t addr) { return controlUnit.setPCAddress(addr); }

void CPU::dumpState() {
    std::cout << "------------- CPU state -------------\n"
              << "PC: 0x" << DisplayOutput::toHex(controlUnit.getPC(), 2)
              << "   IR: 0x" << DisplayOutput::toHex(controlUnit.getIR(), 4)
              << "   Status: " << statusName(status) << '\n';

    for (int row = 0; row < 2; ++row) {
        for (int i = 0; i < 8; ++i) {
            const int reg = row * 8 + i;
            std::cout << 'R' << DisplayOutput::toHex(reg, 1) << '='
                      << DisplayOutput::toHex(registerFile.readRegister(static_cast<uint8_t>(reg)), 2) << "  ";
        }
        std::cout << '\n';
    }
    std::cout << "-------------------------------------\n";
}
