#pragma once

#include <cstdint>
#include <string>

class DisplayOutput {
public:
    static void showByte(uint8_t value);
    static void showCharacter(uint8_t value);
    static std::string toHex(unsigned value, int digits);
};
