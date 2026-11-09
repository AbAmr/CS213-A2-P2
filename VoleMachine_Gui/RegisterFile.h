#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

using namespace std;

class RegisterFile
{
private :
	array<uint8_t, 16> registers;
public:
	//Constructor (all registers = 0)
	RegisterFile();

	//checks if the index is in the range
	bool isValidRegister(uint8_t regIndex);

	//returns register value
	uint8_t read(uint8_t regIndex);

	//overwrites a register by a specific value
	bool write(uint8_t regIndex, uint8_t value);

	//clears the registers
	void reset();

	//for debugging
	void dump();
};

