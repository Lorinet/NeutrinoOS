#pragma once
#include <string>
#include <map>
#include "util.h"
#include "bitconverter.h"
#include "filesystem.h"
#include "instruction.h"
#include "opcode.h"
#include "disassembler.h"
class nvm;
class dynamiclinker
{
public:
	static void dynamicLink(nvm* v);
	static bool link(Array<instruction>* v, map<string, int>* off, map<int, pair<int, int>>* pages, int* ndx, int pid);
	static void replaceModulesByName(Array<instruction>* dasm);
};