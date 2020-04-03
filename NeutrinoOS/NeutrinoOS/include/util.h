#pragma once
#include <vector>
#include <string>
#include <deque>
#define __32BIT
//#define __8BIT
//#define __8BIT_ARDUINO
//#define __ESP32
//#define __WIN32
#define __UNIX
#define __COREOS
typedef unsigned char byte;
using namespace std;
class util
{
public:
	static void remove(vector<byte> vc, int index);
	static vector<byte> initvect(int capacity);
	static string replaceAll(string& str, const string& from, const string& to);
	static bool contains(deque<int> d, int i);
	static bool startsWith(string s, string sta);
};