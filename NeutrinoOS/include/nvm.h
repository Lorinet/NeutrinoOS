#pragma once
#include <map>
#include <stack>
#include "opcode.h"
#include "util.h"
#include "instruction.h"
#include "vt.h"
#include "arrayobj.h"
#include "containers.h"
#include "vmmem.h"
#include "timer.h"
using namespace std;
class vt;
class nvm
{
public:
	int lnkndx;
	Array<instruction>* bytecode;
	IntMap<vmobject>* memory;
	Array<vmobject>* globalPages;
	Array<int>* pageAddresses;
	vmobject* globals;
	vmobject* locals;
	Array<Array<vmobject>>* localScopes;
	Array<int> currentScopes;
	int curPage = 0;
	map<int, arrayobj> arrays;
	map<int, pair<int, int>> pages;
	map<byte, int> eventHandlers;
	Array<ntrevent> eventQueue;
	Array<ntrevent> timerQueue;
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
	bool eventsuspended;
	int processid;
	int inhandler;
	byte processPriority;
	uint16_t curThrd;
	uint32_t pc;
	bool running;
	bool equal;
	bool less;
	bool greater;
	bool zero;
	bool awaitin;
	int waitForProcInput;
	int k, k1, k2, cv1, cv2, addr, cmpi, cmpid, ii, sn;
	unsigned int n;
	Array<byte> v, v1, bl, t, pu, po, arr1, arr2;
	string rep, lnkf;
	Array<instruction> emc;
	byte inter;
	char c;
	instruction i;
	byte* bp;
	long long millis;
	nvm();
	nvm(Array<instruction>* code);
	~nvm();
	void initialize();
	void start();
	void start(int procid, string file);
	void cycle();
	int newobj(vmobject o);
	void branch(int addr);
	void ret();
	void leap(int addr);
	void halt();
	void halt(string err);
	void setTerminals(vt in, vt out);
	void processEvents();
};

