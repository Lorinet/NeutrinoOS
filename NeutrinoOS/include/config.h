#pragma once
#include "util.h"
#include "filesystem.h"
#include "lvmgr.h"
using namespace std;

class config
{
public:
	static string configDirectory;
	static bool keyExists(string key);
	static string getValue(string key);
	static int getValueInt(string key);
	static void setValue(string key, string value);
	static void setValue(string key, int value);
};