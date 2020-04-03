#pragma once
#include <iostream>
#include <string>
#include "util.h"
#include "nvm.h"
#include "vmmgr.h"
#include "vt.h"
using namespace std;
class nvm;
class input
{
public:
	static char readKey();
	static string readLine();
	static void inputLoop(nvm* v);
};