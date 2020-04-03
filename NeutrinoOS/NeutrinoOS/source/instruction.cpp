#include "instruction.h"
instruction::instruction(opcode op, vector<byte> params)
{
	opCode = op;
	parameters = params;
}