#pragma once
#include <vector>
#include "containers.h"
#include "util.h"
using namespace std;
class bitconverter
{
public:
	static int toint32(byte* vc, int index);
	static int toint32(Array<byte> vc, int index);
	static vector<byte> toarray(string s);
	static vector<byte> toarray(int i);
	static Array<byte> toArray(string s);
	static Array<byte> toArray(int i);
	static byte* toarray_p(string s);
	static byte* toarray_p(int i);
	static byte* toarray_p(Array<byte> vb);
	static byte* append(byte* b1, int s1, byte* b2, int s2);
	static string tostring(Array<byte> vc);
	static string tostring(Array<byte> vc, int off);
	static string tostring(byte* vc, int size);
	static string tostring(byte* vc, int off, int size);
	static string readto0(Array<byte> vc, int off);
	static string readto0(byte* vc, int size, int off);
	static Array<byte> subvector(Array<byte> vc, int off);
	static byte* copy(byte* b, int size);
};