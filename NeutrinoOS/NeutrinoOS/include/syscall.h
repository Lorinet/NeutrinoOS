#pragma once
#include <iostream>
#include "containers.h"
#include "util.h"
#include "interrupts.h"
#include "lvmgr.h"
#include "vmmgr.h"
#include "input.h"
#include "sysinfo.h"
#include "ntime.h"
#include "timer.h"
#include "components.h"
#include "iomgr.h"
#include "nfsdimp.h"
using namespace std;
class nvm;
class vt;
class syscall
{
public:
	static string filename, files, folders, txt, txt1, txt2;
	static Array<byte> contents, msg;
	static unsigned int n;
	static int proc, id, id2;
	static byte c;
	static bool bl;
	static vt vi, vo;
	static interrupts syscll;
	static Array<byte> data;
	static Array<byte> systemCall(byte* indata, int datasize, nvm* v);
};