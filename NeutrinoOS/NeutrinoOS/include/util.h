#pragma once
#ifdef __cplusplus
#include <string>
#endif
#define __32BIT
//#define __8BIT
//#define __8BIT_ARDUINO
//#define __ESP32
#define __WIN32
//#define __UNIX
#define __COREOS
typedef unsigned char byte;
#ifdef __cplusplus
using namespace std;
class util
{
public:
	static string replaceAll(string& str, const string& from, const string& to);
	static bool startsWith(string s, string sta);
};
#endif