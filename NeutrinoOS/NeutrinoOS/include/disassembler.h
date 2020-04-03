#pragma once
#include "instruction.h"
#include "util.h"
#include "bitconverter.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
using namespace std;
class disassembler
{
public:
	static vector<instruction> disassembleExecutable(vector<byte> program);
	static vector<instruction> disassembleCode(vector<byte> bc);
	static map<int, int> extractExternalMethods(vector<byte> lib);
};