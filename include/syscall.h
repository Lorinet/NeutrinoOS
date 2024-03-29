#pragma once
#include <iostream>
#include <mutex>
#include "containers.h"
#include "util.h"
#include "interrupts.h"
#include "iapi.h"
using namespace std;
class nvm;
class vt;
class syscall
{
private:
	static mutex syscallMutex;
public:
	static string filename, files, folders, txt, txt1, txt2;
	static Array<byte> contents, msg;
	static unsigned int n;
	static int proc, id, id2;
	static byte c;
	static bool bl;
	static vt vi, vo;
	static interrupts syscll;
	static IntMap<iapi*> interfaces;
	static Array<byte> data;
	static Array<byte> systemCall(byte* indata, int datasize, nvm* v);
};
