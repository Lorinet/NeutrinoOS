#pragma once
#include <vector>
#include "util.h"
using namespace std;
class bitconverter
{
public:
	static int toint32(byte* vc, int index);
	static int toint32(vector<byte> vc, int index);
	static vector<byte> toarray(string s);
	static vector<byte> toarray(int i);
	static string tostring(vector<byte> vc);
	static string tostring(vector<byte> vc, int off);
	static string readto0(vector<byte> vc, int off);
	static vector<byte> subvector(vector<byte> vc, int off);
};