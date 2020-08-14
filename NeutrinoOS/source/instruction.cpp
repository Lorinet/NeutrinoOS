#include "instruction.h"
instruction::instruction()
{
	opCode = opcode::NOP;
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