#pragma once
#include "containers.h"
#include <map>
#include <string>
#include "util.h"
using namespace std;
class lvmgr
{
public:
	static map<string, string> drives;
	static void initialize(string zer0);
	static string formatPath(string path);
	static void mount(string letter, string folder);
	static void unmount(string letter);
	static Array<string> getDrives();
	static bool driveExists(string letter);
};

