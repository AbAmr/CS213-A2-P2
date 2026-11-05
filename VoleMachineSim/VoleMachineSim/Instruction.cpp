#include "Instruction.h"

Instruction::Instruction(uint16_t raw) 
{
    rawInstruction = raw;
}

void Instruction::setInstruction(uint16_t raw)
{
    rawInstruction = raw;
}

uint8_t Instruction::getOpcode() const {
    return (rawInstruction >> 12) & 0x0F;
}

uint8_t Instruction::getR() const 
{
    return (rawInstruction >> 8) & 0x0F;
}

uint8_t Instruction::getX() const 
{
    return (rawInstruction >> 4) & 0x0F;
}

uint8_t Instruction::getY() const
{
    return rawInstruction & 0x0F;
}

uint8_t Instruction::getAddress() const
{
    return rawInstruction & 0xFF;
}