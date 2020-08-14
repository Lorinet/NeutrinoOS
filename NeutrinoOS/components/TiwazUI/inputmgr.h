#pragma once
#include "util.h"
#include "iomgr.h"
#include "viewmgr.h"
#if defined(__UNIX)
#define PIN_BACK 27
#define PIN_ENTER 28
#define PIN_SELECT 29
#elif defined(__ESP32)
#define PIN_BACK 17
#define PIN_ENTER 18
#define PIN_SELECT 19
#endif
using namespace std;
class inputmgr
{
public:
	static int prevStatBack;
	static int prevStatEnter;
	static int prevStatSelect;
	static void initialize();
	static void poll();
};

