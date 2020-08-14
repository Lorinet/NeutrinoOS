#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <time.h>
#include "util.h"
#include "vmmgr.h"
#include "event.h"
using namespace std;
class ntime
{
public:
	static chrono::time_point<chrono::steady_clock> starttmr;
	static int timeZone;
	static void loadNetworkTime();
	static time_t getTime();
	static string getDateTime();
	static string getTimeS();
	static string getTimeWithSeconds();
	static int getHour();
	static int getMinute();
	static int getSecond();
	static int getYear();
	static int getMonth();
	static int getDay();
	static long long getMillis();
	static bool getDST();
	static void setTimeZone(int timeZone);
};