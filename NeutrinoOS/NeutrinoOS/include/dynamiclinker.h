#pragma once
#include <string>
#include <vector>
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
	static void link(vector<instruction>* v, map<string, int>* off, map<int, pair<int, int>>* pages, int* ndx);
	static void replaceModulesByName(vector<instruction>* dasm);
};

class dlmodule
{
public:
	string file;
	int id;
	int offset;
	map<int, int> sections;
	vector<instruction> code;
	dlmodule(string file, int id, int off, map<int, int> sec, vector<instruction> vec);
	static dlmodule getByName(vector<dlmodule> lv, string name);
	static dlmodule getByID(int id);
	static dlmodule getByOffset(int off);
	static bool contains(vector<dlmodule> lv, string name);
};