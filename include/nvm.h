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

struct cflags
{
	bool less;
	bool greater;
	bool equal;
};

class vt;
class nvm
{
public:
	int lnkndx;
	int scopecounter;
	Array<instruction> bytecode;
	ObjectMap memory;
	Array<vmobject> globalPages;
	vmobject* globals;
	vmobject* locals;
	Array<Array<vmobject>> localScopes;
	Array<Array<int>> currentScopes;
	Array<map<int, int>> addrScopes;
	int curPage = 0;
	map<int, arrayobj> arrays;
	map<int, pair<int, int>> pages;
	map<byte, int> eventHandlers;
	Array<ntrevent> eventQueue;
	Array<ntrevent> timerQueue;
	map<int, timerevt> timers;
	BufferedStack astack;
	Array<int> callstack;
	Array<cflags> flagstack;
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
	int pc;
	bool running;
	bool awaitin;
	int waitForProcInput;
	int k, k1, k2;
	unsigned int n;
	Array<byte> v;
	string rep, lnkf;
	Array<instruction> emc;
	byte inter;
	byte* bp;
	long long millis;
	nvm();
	nvm(Array<instruction> code);
	~nvm();
	void initialize();
	void start();
	void start(int procid, string file);
	void cycle();
	int newobj(vmobject o);
	void branch(int addr);
	void iret();
	void ret();
	void leap(int addr);
	void halt();
	void halt(string err);
	void setTerminals(vt in, vt out);
	void processEvents();
	void trash();
	void setpid(int pid);
};

