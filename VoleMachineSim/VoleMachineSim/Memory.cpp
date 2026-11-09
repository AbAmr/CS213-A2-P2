#include <algorithm>
#include "Memory.h"

Memory::Memory() 
{
	cells.resize(256, 0);
}

bool Memory::isValidAddress(uint8_t address) 
{
	return address < 256 ;
}

uint8_t Memory::read(uint8_t address)
{
	if (isValidAddress(address))
	{
		return cells[address];
	}
	return 0;
}

bool Memory::write(uint8_t address, uint8_t value)
{
	if (isValidAddress(address))
	{
		cells[address] = value;
		return true;
	}
	return false;
}

bool Memory::loadProgram(const vector<uint8_t>& programBytes, uint8_t startAddress)
{
	if (startAddress + programBytes.size() > 256)
	{
		cout << "The program is too big to fit in the memory!!!!!!!!!\n";
		return false;
	}


	for (int i = 0; i < programBytes.size() ;i++)
	{
		cells[startAddress + i] = programBytes[i];
	}
	
	return true;
}

void Memory::clear()
{
	fill(cells.begin(), cells.end(), 0);
}

void Memory::dump()
{
	cout << "--- MEMORY DUMP ---\n";
	for (size_t i = 0; i < cells.size(); ++i)
	{
		if (cells[i] != 0)
		{
			cout << "Addr 0x" << hex << (int)i << " : 0x" << (int)cells[i] << "\n"; 
		}
	}
	cout << "-------------------\n";
}

