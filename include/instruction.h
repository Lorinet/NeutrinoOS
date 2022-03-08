#pragma once
#include "opcode.h"
#include "util.h"
#include <vector>
using namespace std;
class instruction
{
public:
	opcode opCode;
	byte* parameters = NULL;
	int psize;
	instruction();
	instruction(opcode op, byte* params, int size);
	instruction(const instruction& other);
	instruction(instruction&& other);
	instruction& operator=(const instruction& other);
	instruction& operator=(instruction&& other);
	~instruction();
};