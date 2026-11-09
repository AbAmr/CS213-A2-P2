#ifndef ALU_H
#define ALU_H

#include <cstdint>

class ALU {
public:
    uint8_t addTwoComplement(uint8_t a, uint8_t b);
    float addFloatingPoint(uint8_t a, uint8_t b);
    uint8_t bitwiseOr(uint8_t a, uint8_t b);
    uint8_t bitwiseAnd(uint8_t a, uint8_t b);
    uint8_t bitwiseXor(uint8_t a, uint8_t b);
    uint8_t rotateRight(uint8_t value, int steps);
    float decimalToFloat(uint8_t value);
    uint8_t floatToDecimal(float value);
};

#endif
