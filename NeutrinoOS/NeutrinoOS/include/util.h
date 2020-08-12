#pragma once
#ifdef __cplusplus
#include <string>
#include <sstream>
#endif
#define __32BIT
//#define __8BIT
//#define __8BIT_ARDUINO
//#define __ESP32
#define __WIN32
#define __X86
//#define __UNIX
//#define __HOLO
typedef unsigned char byte;
#ifdef __cplusplus
using namespace std;
class util
{
public:
	static string replaceAll(string& str, const string& from, const string& to);
	static string getLast(string str, char del);
	static bool startsWith(string s, string sta);
};
#endif