#include "RegisterFile.h"

RegisterFile::RegisterFile()
{
	for (int i = 0; i < 16; i++)
	{
		registers[i] = 0;
	}
}

bool RegisterFile::isValidRegister(uint8_t regIndex)
{
	return regIndex < 16;
}

uint8_t RegisterFile::read(uint8_t regIndex)
{
	if (isValidRegister(regIndex))
	{
		return registers[regIndex];
	}

	return 0;
}

bool RegisterFile::write(uint8_t regIndex, uint8_t value)
{
	if (isValidRegister(regIndex))
	{
		registers[regIndex] = value;
		return true;
	}

	return false;
}

void RegisterFile::reset()
{
	for (int i = 0; i < 16; i++)
	{
		registers[i] = 0;
	}
}

void RegisterFile::dump()
{
	cout << "--- REGISTER FILE DUMP ---\n";
	for (size_t i = 0; i < registers.size(); ++i) 
	{
		cout << "R" << hex << uppercase << i << " : 0x" <<  (int)registers[i] << "\n";
	}
	cout << "--------------------------\n";
}
