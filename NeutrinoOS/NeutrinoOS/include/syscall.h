#pragma once
#include <iostream>
#include <vector>
#include "util.h"
#include "interrupts.h"
#include "lvmgr.h"
#include "vmmgr.h"
#include "input.h"
#include "textscreen.h"
#include "sysinfo.h"
#include "ntime.h"
#include "timer.h"
#include "components.h"
#include "iomgr.h"
#include "nfsdimp.h"
using namespace std;
class nvm;
class syscall
{
public:
	static vector<byte> systemCall(vector<byte> indata, nvm* v);
};