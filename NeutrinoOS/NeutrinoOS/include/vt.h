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
	deque<vector<byte>> buffer;
	vector<byte> input;
	vt();
	vt(int proc, int targ, vtype vty);
	void write(string data);
	void write(vector<byte> data);
	vector<byte> readLine();
	vector<byte> readAllData();
	string readLineStr();
	string readAllStr();
	void waitForInput();
	void pressKey(byte b);
	void registerInputData(vector<byte> data);
	void registerInputString(string data);
};