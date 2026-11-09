#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

using namespace std;

class Memory
{
private:
	vector<uint8_t> cells;

public:
	//Constructor (all memory values = 0)
	Memory();
	
	//checks if the address is in the range
	bool isValidAddress(uint8_t address);

	//reads a 1 byte from the memory 
	uint8_t read(uint8_t address);

	//overwrite a 1 byte value into a specific memory address
	bool write(uint8_t address, uint8_t value);

	//loads a sequence of program bytes into the memory starting from a specific address
	//param programBytes: a vector of bytes representing the machine instructions/data (every cell in the vector has a 1 byte memory)
	//param startAddress: the memory address where the program loading begins
	//return bool: returns true if loaded successfully, false if it exceeds memory size
	bool loadProgram(const vector<uint8_t> & programBytes, uint8_t startAddress);

	//clears the memory
	void clear();

	//Display the non zero values in the memory (onlu for debugging!!!!!)
	void dump();
};

