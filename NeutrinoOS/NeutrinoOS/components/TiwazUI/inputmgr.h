#pragma once
extern "C" 
{ 
#include <wiringPi.h> 
}
#include "viewmgr.h"
#define PIN_BACK 27
#define PIN_ENTER 28
#define PIN_SELECT 29
using namespace std;
class inputmgr
{
public:
	static int prevStatBack;
	static int prevStatEnter;
	static int prevStatSelect;
	static void initialize();
	static int readPinState(int pin);
	static void writePinState(int pin, int state);
	static void poll();
};

