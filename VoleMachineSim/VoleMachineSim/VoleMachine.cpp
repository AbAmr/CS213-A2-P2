#include "VoleMachine.h"

#include <algorithm>
#include <iostream>

#include "DisplayOutput.h"

namespace {
const unsigned MEMORY_SIZE = 256;
}

VoleMachine::VoleMachine() {}

bool VoleMachine::loadProgram(const std::vector<uint8_t>& program, uint8_t startAddress) {
    if (!memory.loadProgram(program, startAddress))
        return false;
    cpu.reset();
    return cpu.setProgramCounter(startAddress);
}

ExecutionStatus VoleMachine::step() { return cpu.step(memory); }
ExecutionStatus VoleMachine::run()  { return cpu.run(memory); }

void VoleMachine::reset() {
    cpu.reset();
    memory.clear();
}

void VoleMachine::displayStatus() {
    cpu.dumpState();

    switch (cpu.getStatus()) {
        case ExecutionStatus::HALTED:
            std::cout << "Result: program halted normally (HALT reached).\n";
            break;
        case ExecutionStatus::ERROR: {
            ExecutionError err = cpu.getError();
            std::cout << "Result: ERROR " << err.getCode() << " - " << err.getMessage() << '\n';
            break;
        }
        default:
            std::cout << "Result: ready to execute the next instruction.\n";
    }
}

void VoleMachine::dumpMemory(uint8_t startAddress, uint16_t count) {
    const unsigned end = std::min<unsigned>(startAddress + count, MEMORY_SIZE);
    if (count == 0 || startAddress >= end) {
        std::cout << "Nothing to dump.\n";
        return;
    }

    std::cout << "Memory 0x" << DisplayOutput::toHex(startAddress, 2)
              << " - 0x" << DisplayOutput::toHex(end - 1, 2) << '\n';

    for (unsigned addr = startAddress; addr < end; ++addr) {
        if ((addr - startAddress) % 16 == 0)
            std::cout << (addr == startAddress ? "" : "\n") << DisplayOutput::toHex(addr, 2) << ":";
        std::cout << ' ' << DisplayOutput::toHex(memory.readAddress(static_cast<uint8_t>(addr)), 2);
    }
    std::cout << '\n';
}
