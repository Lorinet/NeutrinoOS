#pragma once
#include "instruction.h"
#include "util.h"
#include "bitconverter.h"
#include "containers.h"
#include <map>
#include <string>
#include <iostream>
using namespace std;
class disassembler
{
public:
	static Array<instruction>* disassembleExecutable(byte* program, int size);
	static Array<instruction>* disassembleCode(byte* bc, int size);
	static map<int, int> extractExternalMethods(byte* lib, int size);
};