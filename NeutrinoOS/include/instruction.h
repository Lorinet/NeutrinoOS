#pragma once
#include "opcode.h"
#include "util.h"
#include <vector>
using namespace std;
class instruction
{
public:
	opcode opCode;
	byte* parameters;
	int psize;
	instruction();
	instruction(opcode op, byte* params, int size);
};