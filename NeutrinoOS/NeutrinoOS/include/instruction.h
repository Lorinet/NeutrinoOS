#pragma once
#include "opcode.h"
#include "util.h"
#include <vector>
using namespace std;
class instruction
{
public:
	opcode opCode;
	vector<byte> parameters;
	instruction(opcode op, vector<byte> params);
};