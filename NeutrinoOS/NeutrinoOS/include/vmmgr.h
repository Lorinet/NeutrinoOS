#pragma once
#include <algorithm>
#include <vector>
#include <map>
#include "filesystem.h"
#include "event.h"
#include "nvm.h"
using namespace std;
class nvm;
class vt;
class vmmgr
{
public:
	static bool running;
	static map<int, nvm> processes;
	static vector<int> procidx;
	static int activeProcess;
	static int prevActiveProcess;
	static void start();
	static void stepAll();
	static int startProgram(string path, bool active);
	static int createProcess(string file, bool active);
	static int createProcessEx(string file, vt in, vt out);
	static void terminateProcess(int pid);
	static void sendMessage(int pid, vector<byte> msg);
	static void sendInput(int pid, vector<byte> input);
	static void reload();
	static void suspend();
	static void vmmerror(string error);
	static void vmmerror(string error, int procid);
	static bool inputRequested(int pid);
};