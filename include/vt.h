#pragma once
#include <string>
#include <vector>
#include <deque>
#include "vmmgr.h"
#include "util.h"
#include "vtype.h"
using namespace std;
class vt
{
public:
	int process;
	int target;
	vtype type;
	Array<byte> input;
	vt();
	vt(int proc, int targ, vtype vty);
	void write(string data);
	void write(Array<byte> data);
	void waitForInput();
	void pressKey(byte b);
	void registerInputData(Array<byte> data);
	void registerInputString(string data);
};