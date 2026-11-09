#include "DisplayOutput.h"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

void DisplayOutput::showByte(uint8_t value) {
    std::cout << "[SCREEN] byte: 0x" << toHex(value, 2)
              << " (" << static_cast<unsigned>(value) << ")\n";
}

void DisplayOutput::showCharacter(uint8_t value) {
    std::cout << "[SCREEN] char: ";
    if (std::isprint(value))
        std::cout << '\'' << static_cast<char>(value) << '\'';
    else
        std::cout << "(non-printable)";
    std::cout << '\n';
}

std::string DisplayOutput::toHex(unsigned value, int digits) {
    std::ostringstream out;
    out << std::uppercase << std::hex << std::setw(digits) << std::setfill('0') << value;
    return out.str();
}
