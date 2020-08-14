#pragma once
#include "containers.h"
#include <string>
#include "util.h"
#include "vmmgr.h"
#include "ntime.h"
#include "timer.h"
using namespace std;
class ntrevent
{
public:
	byte id;
	Array<Array<byte>> parameters;
	ntrevent();
	ntrevent(byte i, Array<Array<byte>> p);
};

enum eventid
{
	NoEvent = 0x00,
	KeyDown = 0x01,
	KeyUp = 0x02,
	PinChanged3 = 0x03,
	PinChanged4 = 0x04,
	PinChanged5 = 0x05,
	PinChanged6 = 0x06,
	PinChanged7 = 0x07,
	PinChanged8 = 0x08,
	PinChanged9 = 0x09,
	PinChanged10 = 0x0A,
	PinChanged11 = 0x0B,
	PinChanged12 = 0x0C,
	PinChanged13 = 0x0D,
	PinChanged14 = 0x0E,
	PinChanged15 = 0x0F,
	PinChanged16 = 0x10,
	PinChanged17 = 0x11,
	PinChanged18 = 0x12,
	PinChanged19 = 0x13,
	PinChanged20 = 0x14,
	PinChanged21 = 0x15,
	PinChanged22 = 0x16,
	PinChanged23 = 0x17,
	PinChanged24 = 0x18,
	PinChanged25 = 0x19,
	PinChanged26 = 0x1A,
	PinChanged27 = 0x1B,
	PinChanged28 = 0x1C,
	PinChanged29 = 0x1D,
	PinChanged30 = 0x1E,
	PinChanged31 = 0x1F,
	PinChanged32 = 0x20,
	PinChanged33 = 0x21,
	PinChanged34 = 0x22,
	PinChanged35 = 0x23,
	PinChanged36 = 0x24,
	PinChanged37 = 0x25,
	PinChanged38 = 0x26,
	PinChanged39 = 0x27,
	TimeChangeSeconds = 0x28,
	TimeChangeMinutes = 0x29,
	TimeChangeHours = 0x2A,
	TimeSet = 0x2B
};
class events
{
public:
	static Array<ntrevent> pending;
	static int seconds;
	static int minutes;
	static int hours;
	static bool timeSet;
	static void eventLoop();
};