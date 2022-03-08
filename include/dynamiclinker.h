#pragma once
#include <string>
#include <map>
#include "util.h"
#include "vmmem.h"
#include "instruction.h"
#include "containers.h"
class nvm;
class vmpage;
class dynamiclinker
{
public:

	static void dynamicLink(nvm* v);
	static bool link(Array<instruction>& v, map<string, int>& off, map<int, pair<int, int>>& pages, int& ndx, int pid);
	static void replaceModulesByName(Array<instruction>& dasm);
};