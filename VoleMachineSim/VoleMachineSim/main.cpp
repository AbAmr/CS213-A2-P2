#include <cctype>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "DisplayOutput.h"
#include "VoleMachine.h"

namespace {

const uint8_t DEFAULT_START_ADDRESS = 0x0A;

std::string trim(const std::string& text) {
    const size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

bool isHexString(const std::string& text) {
    if (text.empty()) return false;
    for (char c : text)
        if (!std::isxdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

std::string stripHexPrefix(const std::string& token) {
    if (token.size() > 2 && token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
        return token.substr(2);
    return token;
}

bool parseHexByte(const std::string& text, uint8_t& value) {
    const std::string digits = stripHexPrefix(trim(text));
    if (!isHexString(digits) || digits.size() > 2) return false;
    value = static_cast<uint8_t>(std::stoi(digits, nullptr, 16));
    return true;
}

bool parseCount(const std::string& text, uint16_t& value) {
    const std::string digits = trim(text);
    if (digits.empty() || digits.size() > 4) return false;
    for (char c : digits)
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    value = static_cast<uint16_t>(std::stoi(digits));
    return true;
}

bool ask(const std::string& prompt, std::string& answer) {
    std::cout << prompt;
    if (!std::getline(std::cin, answer)) return false;
    answer = trim(answer);
    return true;
}

bool readProgramFile(const std::string& path, std::vector<uint8_t>& program, std::string& errorText) {
    std::ifstream file(path);
    if (!file) {
        errorText = "cannot open file '" + path + "'";
        return false;
    }

    std::string digits;
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        for (const char* marker : {"#", ";", "//"}) {
            const size_t pos = line.find(marker);
            if (pos != std::string::npos) line.erase(pos);
        }
        std::istringstream tokens(line);
        std::string token;
        while (tokens >> token) {
            const std::string hex = stripHexPrefix(token);
            if (!isHexString(hex)) {
                errorText = "invalid hex value '" + token + "' on line " + std::to_string(lineNumber);
                return false;
            }
            digits += hex;
        }
    }

    if (digits.empty()) {
        errorText = "the file contains no program data";
        return false;
    }
    if (digits.size() % 2 != 0) {
        errorText = "odd number of hex digits (" + std::to_string(digits.size()) +
                    "); every byte needs two digits";
        return false;
    }

    program.clear();
    for (size_t i = 0; i < digits.size(); i += 2)
        program.push_back(static_cast<uint8_t>(std::stoi(digits.substr(i, 2), nullptr, 16)));
    return true;
}

bool loadFromFile(VoleMachine& machine, const std::string& path, uint8_t startAddress) {
    std::vector<uint8_t> program;
    std::string errorText;
    if (!readProgramFile(path, program, errorText)) {
        std::cout << "Load failed: " << errorText << '\n';
        return false;
    }
    if (!machine.loadProgram(program, startAddress)) {
        std::cout << "Load failed: " << program.size() << " byte(s) do not fit in memory starting at 0x"
                  << DisplayOutput::toHex(startAddress, 2) << ".\n";
        return false;
    }
    std::cout << "Loaded " << program.size() << " byte(s) at 0x" << DisplayOutput::toHex(startAddress, 2)
              << ". PC set to 0x" << DisplayOutput::toHex(startAddress, 2) << ".\n";
    return true;
}

bool menuLoad(VoleMachine& machine) {
    std::string path, startText;
    if (!ask("Program file (.txt): ", path) || path.empty()) {
        std::cout << "Cancelled.\n";
        return false;
    }
    uint8_t start = DEFAULT_START_ADDRESS;
    if (ask("Start address in hex [" + DisplayOutput::toHex(DEFAULT_START_ADDRESS, 2) + "]: ", startText) &&
        !startText.empty() && !parseHexByte(startText, start)) {
        std::cout << "Invalid address. Use 1-2 hex digits (00-FF).\n";
        return false;
    }
    return loadFromFile(machine, path, start);
}

void menuDump(VoleMachine& machine) {
    std::string startText, countText;
    uint8_t start = 0x00;
    uint16_t count = 256;
    if (ask("Start address in hex [00]: ", startText) && !startText.empty() && !parseHexByte(startText, start)) {
        std::cout << "Invalid address.\n";
        return;
    }
    if (ask("Number of cells in decimal [256]: ", countText) && !countText.empty() && !parseCount(countText, count)) {
        std::cout << "Invalid count.\n";
        return;
    }
    machine.dumpMemory(start, count);
}

void printMenu() {
    std::cout << "\n==== Vole Machine Simulator ====\n"
              << " 1) Load program from a .txt file\n"
              << " 2) Run program (until HALT or error)\n"
              << " 3) Execute a single step\n"
              << " 4) Show CPU status (PC, IR, registers)\n"
              << " 5) Dump memory\n"
              << " 6) Reset machine (clears memory)\n"
              << " 0) Exit\n"
              << "Choice: ";
}

}

int main(int argc, char* argv[]) {
    VoleMachine machine;
    bool programLoaded = false;

    if (argc > 1)
        programLoaded = loadFromFile(machine, argv[1], DEFAULT_START_ADDRESS);

    std::string choice;
    while (true) {
        printMenu();
        if (!std::getline(std::cin, choice)) break;
        choice = trim(choice);

        if (choice == "1") {
            if (menuLoad(machine)) programLoaded = true;
        } else if (choice == "2" || choice == "3") {
            if (!programLoaded) {
                std::cout << "Load a program first (option 1).\n";
            } else {
                if (choice == "2") machine.run(); else machine.step();
                machine.displayStatus();
            }
        } else if (choice == "4") {
            machine.displayStatus();
        } else if (choice == "5") {
            menuDump(machine);
        } else if (choice == "6") {
            machine.reset();
            programLoaded = false;
            std::cout << "Machine reset: registers, PC and memory cleared.\n";
        } else if (choice == "0") {
            break;
        } else {
            std::cout << "Invalid choice, please enter 0-6.\n";
        }
    }
    std::cout << "Goodbye!\n";
    return 0;
}
