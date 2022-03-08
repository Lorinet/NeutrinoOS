#include "instruction.h"

instruction::instruction()
{
	opCode = opcode::OP_NOP;
	parameters = NULL;
	psize = 0;
}

instruction::instruction(opcode op, byte* params, int size)
{
	opCode = op;
	parameters = new byte[size];
	for (int i = 0; i < size; i++) parameters[i] = params[i];
	psize = size;
}

instruction::instruction(const instruction& other)
{
	operator=(other);
}

instruction::instruction(instruction&& other)
{
	operator=(other);
}

instruction& instruction::operator=(const instruction& other)
{
	opCode = other.opCode;
	psize = other.psize;
	delete[] parameters;
	parameters = new byte[psize];
	for(int i = 0; i < psize; i++) parameters[i] = other.parameters[i];
	return *this;
}

instruction& instruction::operator=(instruction&& other)
{
	opCode = other.opCode;
	psize = other.psize;
	delete[] parameters;
	parameters = new byte[psize];
	for(int i = 0; i < psize; i++) parameters[i] = other.parameters[i];
	other.opCode = opcode::OP_NOP;
	other.psize = 0;
	delete[] other.parameters;
	other.parameters = NULL;
	return *this;
}

instruction::~instruction()
{
	delete[] parameters;
	parameters = NULL;
}