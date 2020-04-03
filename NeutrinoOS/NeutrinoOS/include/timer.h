#pragma once
#include "util.h"
#include "ntime.h"
using namespace std;
class timerevt
{
public:
	long long startTime;
	int interval;
	bool loop;
	bool running;
	int handler;
	timerevt(int ival, int hndl);
	timerevt();
};