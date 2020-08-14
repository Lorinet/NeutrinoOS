#pragma once
#ifdef __cplusplus
#include <string>
#include <algorithm>
#include <sstream>
#endif
#include "platformconfig.h"
#include "sysinfo.h"
typedef unsigned char byte;
#ifdef __cplusplus
using namespace std;
class util
{
public:
	static string replaceAll(string str, string from, string to);
	static string getLast(string str, char del);
	static bool startsWith(string s, string sta);
	static string combinePath(string p1, string p2);
	static string pathGetParent(string path);
	static string pathGetLast(string path);
};
#endif