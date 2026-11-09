#include "ControlUnit.h"

#include <string>

#include "ALU.h"
#include "DisplayOutput.h"
#include "Memory.h"
#include "RegisterFile.h"

namespace {
bool fail(ExecutionError& error, const std::string& message) {
    error.setError(message);
    return false;
}
}

ControlUnit::ControlUnit() : pc(0), ir(0), halted(false) {}

Instruction ControlUnit::fetch(Memory& memory, ExecutionError& error) {
    if (pc == 0xFF) {
        error.setError("PC is 0xFF: no room for a 2-byte instruction at the end of memory");
        return Instruction(0);
    }

    const uint8_t address = pc;
    const uint8_t high = memory.read(address);
    const uint8_t low  = memory.read(static_cast<uint8_t>(address + 1));

    ir = static_cast<uint16_t>((high << 8) | low);
    pc = static_cast<uint8_t>(pc + 2);

    Instruction instruction(ir);
    const uint8_t opcode = instruction.getOpcode();
    if (opcode < 0x1 || opcode > 0xC) {
        error.setError("Invalid instruction 0x" + DisplayOutput::toHex(ir, 4) +
                       " at address 0x" + DisplayOutput::toHex(address, 2));
    }
    return instruction;
}

bool ControlUnit::execute(Instruction instruction, RegisterFile& regFile, ALU& alu,
                          Memory& memory, ExecutionError& error) {
    const uint8_t opcode = instruction.getOpcode();
    const uint8_t r  = instruction.getR();
    const uint8_t s  = instruction.getX();
    const uint8_t t  = instruction.getY();
    const uint8_t xy = instruction.getAddress();

    if (!regFile.isValidRegister(r) || !regFile.isValidRegister(s) || !regFile.isValidRegister(t))
        return fail(error, "Instruction 0x" + DisplayOutput::toHex(ir, 4) + " uses a register that does not exist");

    switch (opcode) {
    case 0x1:
        if (!memory.isValidAddress(xy))
            return fail(error, "LOAD from invalid address 0x" + DisplayOutput::toHex(xy, 2));
        regFile.write(r, memory.read(xy));
        break;

    case 0x2:
        regFile.write(r, xy);
        break;

    case 0x3:
        if (!memory.write(xy, regFile.read(r)))
            return fail(error, "STORE to invalid address 0x" + DisplayOutput::toHex(xy, 2));
        if (xy == 0x00) {
            DisplayOutput::showByte(regFile.read(r));
            DisplayOutput::showCharacter(regFile.read(r));
        }
        break;

    case 0x4:
        regFile.write(t, regFile.read(s));
        break;

    case 0x5:
        regFile.write(r, alu.addTwoComplement(regFile.read(s), regFile.read(t)));
        break;

    case 0x6:
        regFile.write(r, alu.floatToDecimal(alu.addFloatingPoint(regFile.read(s), regFile.read(t))));
        break;

    case 0x7:
        regFile.write(r, alu.bitwiseOr(regFile.read(s), regFile.read(t)));
        break;

    case 0x8:
        regFile.write(r, alu.bitwiseAnd(regFile.read(s), regFile.read(t)));
        break;

    case 0x9:
        regFile.write(r, alu.bitwiseXor(regFile.read(s), regFile.read(t)));
        break;

    case 0xA:
        regFile.write(r, alu.rotateRight(regFile.read(r), t));
        break;

    case 0xB:
        if (regFile.read(r) == regFile.read(0) && !setPCAddress(xy))
            return fail(error, "JUMP to invalid address 0x" + DisplayOutput::toHex(xy, 2));
        break;

    case 0xC:
        halted = true;
        break;

    default:
        return fail(error, "Unknown opcode " + DisplayOutput::toHex(opcode, 1));
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
