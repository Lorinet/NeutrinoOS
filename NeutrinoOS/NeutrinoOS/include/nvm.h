#pragma once
#include <vector>
#include <map>
#include <stack>
#include <deque>
#include "opcode.h"
#include "util.h"
#include "syscall.h"
#include "disassembler.h"
#include "bitconverter.h"
#include "instruction.h"
#include "event.h"
#include "dynamiclinker.h"
#include "timer.h"
#include "vt.h"
#include "arrayobj.h"
using namespace std;
class vt;
class timerevt;
class nvm
{
public:
	int lnkndx;
	map<string, pair<int, int>> modules;
	map<int, int> extcalls;
	vector<instruction> bytecode;
	map<int, map<int, vector<byte>>> memory;
	map<int, arrayobj> arrays;
	int curPage;
	map<int, pair<int, int>> pages;
	map<byte, int> eventHandlers;
	map<int, timerevt> timers;
	stack<vector<byte>> astack;
	deque<int> callstack;
	stack<vector<byte>> messages;
	vt* interm;
	vt* outterm;
	bool awaitmsg;
	bool eventsenabled;
	bool suspended;
	int processid;
	unsigned int pc;
	bool running;
	bool equal;
	bool less;
	bool greater;
	bool zero;
	bool awaitin;
	byte bits;
	int waitForProcInput;
	nvm();
	nvm(vector<instruction> code);
	void start();
	void start(int procid);
	void cycle();
	void branch(int addr);
	void halt();
	void halt(string err);
};

