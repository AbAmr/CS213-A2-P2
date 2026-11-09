#include "ControlUnit.h"

#include <string>

#include "ALU.h"
#include "DisplayOutput.h"
#include "Memory.h"
#include "RegisterFile.h"

namespace {
bool fail(ExecutionError& error, int code, const std::string& message) {
    error.setError(code, message);
    return false;
}
}

ControlUnit::ControlUnit() : pc(0), ir(0), halted(false) {}

Instruction ControlUnit::fetch(Memory& memory, ExecutionError& error) {
    if (pc == 0xFF) {
        error.setError(VoleError::PC_OUT_OF_RANGE,
                       "PC is 0xFF: no room for a 2-byte instruction at the end of memory");
        return Instruction(0);
    }

    const uint8_t address = pc;
    const uint8_t high = memory.readAddress(address);
    const uint8_t low  = memory.readAddress(static_cast<uint8_t>(address + 1));

    ir = static_cast<uint16_t>((high << 8) | low);
    pc = static_cast<uint8_t>(pc + 2);

    Instruction instruction(ir);
    if (!instruction.decode() || !instruction.isValid()) {
        error.setError(VoleError::INVALID_INSTRUCTION,
                       "Invalid instruction 0x" + DisplayOutput::toHex(ir, 4) +
                       " at address 0x" + DisplayOutput::toHex(address, 2));
    }
    return instruction;
}

bool ControlUnit::execute(Instruction instruction, RegisterFile& regFile, ALU& alu,
                          Memory& memory, ExecutionError& error) {
    const uint8_t opcode = instruction.getOpcode();
    const uint8_t r  = instruction.getR();
    const uint8_t s  = instruction.getS();
    const uint8_t t  = instruction.getT();
    const uint8_t y  = instruction.getY();
    const uint8_t xy = static_cast<uint8_t>((instruction.getX() << 4) | y);

    if (!regFile.isValidRegister(r) || !regFile.isValidRegister(s) || !regFile.isValidRegister(t))
        return fail(error, VoleError::INVALID_REGISTER,
                    "Instruction 0x" + DisplayOutput::toHex(ir, 4) + " uses a register that does not exist");

    switch (opcode) {
    case 0x1:
        if (!memory.isValidAddress(xy))
            return fail(error, VoleError::INVALID_ADDRESS, "LOAD from invalid address 0x" + DisplayOutput::toHex(xy, 2));
        regFile.writeRegister(r, memory.readAddress(xy));
        break;

    case 0x2:
        regFile.writeRegister(r, xy);
        break;

    case 0x3:
        if (!memory.writeAddress(xy, regFile.readRegister(r)))
            return fail(error, VoleError::INVALID_ADDRESS, "STORE to invalid address 0x" + DisplayOutput::toHex(xy, 2));
        break;

    case 0x4:
        regFile.writeRegister(t, regFile.readRegister(s));
        break;

    case 0x5:
        regFile.writeRegister(r, alu.addTwosComplement(regFile.readRegister(s), regFile.readRegister(t)));
        break;

    case 0x6:
        regFile.writeRegister(r, alu.addFloatingPoint(regFile.readRegister(s), regFile.readRegister(t)));
        break;

    case 0x7:
        regFile.writeRegister(r, alu.bitwiseOr(regFile.readRegister(s), regFile.readRegister(t)));
        break;

    case 0x8:
        regFile.writeRegister(r, alu.bitwiseAnd(regFile.readRegister(s), regFile.readRegister(t)));
        break;

    case 0x9:
        regFile.writeRegister(r, alu.bitwiseXor(regFile.readRegister(s), regFile.readRegister(t)));
        break;

    case 0xA:
        regFile.writeRegister(r, alu.rotateRight(regFile.readRegister(r), y));
        break;

    case 0xB:
        if (regFile.readRegister(r) == regFile.readRegister(0) && !setPCAddress(xy))
            return fail(error, VoleError::INVALID_JUMP, "JUMP to invalid address 0x" + DisplayOutput::toHex(xy, 2));
        break;

    case 0xC:
        halted = true;
        break;

    default:
        return fail(error, VoleError::INVALID_INSTRUCTION,
                    "Unknown opcode " + DisplayOutput::toHex(opcode, 1));
    }
    return true;
}

bool ControlUnit::isHalted() const            { return halted; }
void ControlUnit::setHalted(bool value)       { halted = value; }
uint8_t  ControlUnit::getPC() const           { return pc; }
uint16_t ControlUnit::getIR() const           { return ir; }

void ControlUnit::reset() {
    pc = 0;
    ir = 0;
    halted = false;
}

bool ControlUnit::setPCAddress(uint8_t address) {
    if (address == 0xFF) return false;
    pc = address;
    return true;
}
