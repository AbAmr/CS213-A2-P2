#include "ALU.h"
#include <cmath>

uint8_t ALU::addTwoComplement(uint8_t a, uint8_t b) {
    int sum = static_cast<int>(static_cast<int8_t>(a)) + static_cast<int>(static_cast<int8_t>(b));
    return static_cast<uint8_t>(sum & 0xFF);
}

float ALU::addFloatingPoint(uint8_t a, uint8_t b) {
    float valA = decimalToFloat(a);
    float valB = decimalToFloat(b);
    return valA + valB;
}

uint8_t ALU::bitwiseOr(uint8_t a, uint8_t b) {
    return a | b;
}

uint8_t ALU::bitwiseAnd(uint8_t a, uint8_t b) {
    return a & b;
}

uint8_t ALU::bitwiseXor(uint8_t a, uint8_t b) {
    return a ^ b;
}

uint8_t ALU::rotateRight(uint8_t value, int steps) {
    steps %= 8;
    if (steps < 0) steps += 8;
    return (value >> steps) | (value << (8 - steps));
}

float ALU::decimalToFloat(uint8_t value) {
    int sign = (value & 0x80) ? -1 : 1;
    int exponent = (value >> 4) & 0x07;
    int mantissa = value & 0x0F;

    float fraction = 1.0f + (static_cast<float>(mantissa) / 16.0f);
    return sign * fraction * std::pow(2.0f, exponent - 4);
}

uint8_t ALU::floatToDecimal(float value) {
    if (value == 0.0f) return 0;
    int sign = (value < 0) ? 1 : 0;
    float absVal = std::abs(value);

    int exp = 0;
    while (absVal >= 2.0f && exp < 7) {
        absVal /= 2.0f;
        exp++;
    }
    while (absVal < 1.0f && exp > -4) {
        absVal *= 2.0f;
        exp--;
    }

    int mantissa = static_cast<int>((absVal - 1.0f) * 16.0f);
    if (mantissa > 15) mantissa = 15;
    if (mantissa < 0) mantissa = 0;

    uint8_t result = (sign << 7) | ((exp + 4) << 4) | mantissa;
    return result;
}