#include "timer.h"
timerevt::timerevt(int ival, int hndl)
{
	startTime = ntime::getMillis();
	interval = ival;
	handler = hndl;
	running = false;
	loop = false;
}

timerevt::timerevt()
{
	startTime = ntime::getMillis();
	interval = 0;
	handler = 0;
	running = false;
	loop = false;
}