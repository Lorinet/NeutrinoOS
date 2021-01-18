#pragma once
#include <map>
#include <stack>
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
#include "containers.h"
#include "memorystats.h"
using namespace std;
class vt;
class timerevt;
class nvm
{
public:
	int lnkndx;
	map<string, pair<int, int>> modules;
	map<int, int> extcalls;
	Array<instruction>* bytecode;
	map<int, map<int, Array<byte>>> memory;
	map<int, arrayobj> arrays;
	map<int, map<int, Array<byte>>> objects;
	int curPage;
	map<int, pair<int, int>> pages;
	map<byte, int> eventHandlers;
	map<int, timerevt> timers;
	BufferedStack astack;
	Array<int> callstack;
	Array<Array<byte>> messages;
	string fileName;
	vt* interm;
	vt* outterm;
	bool awaitmsg;
	bool eventsenabled;
	bool suspended;
	int processid;
	byte processPriority;
	unsigned int pc;
	bool running;
	bool equal;
	bool less;
	bool greater;
	bool zero;
	bool awaitin;
	byte bits;
	int waitForProcInput;
	int k, k1, k2, cv1, cv2, addr, cmpi, cmpid, ii, sn;
	unsigned int n;
	Array<byte> v, v1, bl, t, pu, po, arr1, arr2;
	map<int, int> sectionMap;
	string rep, lnkf;
	Array<instruction> emc;
	byte inter;
	char c;
	instruction i;
	byte* bp;
	long long millis;
	nvm();
	nvm(Array<instruction>* code);
	void start();
	void start(int procid, string file);
	void cycle();
	void branch(int addr);
	void halt();
	void halt(string err);
	void setTerminals(vt in, vt out);
};

