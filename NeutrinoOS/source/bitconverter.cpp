#include "bitconverter.h"
int bitconverter::toint32(byte* vc, int index)
{
	return vc[index + 3] << 24 | vc[index + 2] << 16 | vc[index + 1] << 8 | vc[index];
}
int bitconverter::toint32(Array<byte> vc, int index)
{
	return vc[index + 3] << 24 | vc[index + 2] << 16 | vc[index + 1] << 8 | vc[index];
}
vector<byte> bitconverter::toarray(string s)
{
	vector<byte> a;
	for (char c : s)
	{
		a.push_back(c);
	}
	return a;
}
vector<byte> bitconverter::toarray(int i)
{
	vector<byte> a;
	a.push_back(i);
	a.push_back(i >> 8);
	a.push_back(i >> 16);
	a.push_back(i >> 24);
	return a;
}
Array<byte> bitconverter::toArray(string s)
{
	Array<byte> a;
	for (char c : s)
	{
		a.push(c);
	}
	return a;
}
Array<byte> bitconverter::toArray(int i)
{
	Array<byte> a;
	a.push(i);
	a.push(i >> 8);
	a.push(i >> 16);
	a.push(i >> 24);
	return a;
}
byte* bitconverter::copy(byte* b, int size)
{
	byte* n = new byte[size];
	for (int i = 0; i < size; i++) n[i] = b[i];
	return n;
}
byte* bitconverter::toarray_p(int i)
{
	byte* a = new byte[4];
	a[0] = i;
	a[1] = i >> 8;
	a[2] = i >> 16;
	a[3] = i >> 24;
	return a;
}
byte* bitconverter::toarray_p(string s)
{
	int l = s.size();
	byte* a = new byte[l];
	for(int i = 0; i < l; i++) a[i] = s[i];
	return a;
}
byte* bitconverter::toarray_p(Array<byte> vb)
{
	int l = vb.size;
	byte* b = new byte[l];
	for (int i = 0; i < l; i++) b[i] = vb[i];
	return b;
}
byte* bitconverter::append(byte* b1, int s1, byte* b2, int s2)
{
	byte* a = new byte[s1 + s2];
	for(int i = 0; i < s1; i++) a[i] = b1[i];
	for(int i = 0; i < s2; i++) a[i + s1] = b2[i];
	return a;
}
string bitconverter::tostring(Array<byte> vc)
{
	string s = "";
	for (int i = 0; i < vc.size; i++)
	{
		s += vc[i];
	}
	return s;
}
string bitconverter::tostring(byte* vc, int size)
{
	string s = "";
	for (int i = 0; i < size; i++)
	{
		s += vc[i];
	}
	return s;
}
string bitconverter::tostring(Array<byte> vc, int off)
{
	string s = "";
	for (int i = off; i < vc.size; i++)
	{
		s += vc[i];
	}
	return s;
}
string bitconverter::tostring(byte* vc, int off, int size)
{
	string s = "";
	for (int i = off; i < size; i++)
	{
		s += vc[i];
	}
	return s;
}
string bitconverter::readto0(Array<byte> vc, int off)
{
	string s = "";
	for (int i = off; i < vc.size; i++)
	{
		if (vc[i] == 0) break;
		s += vc[i];
	}
	return s;
}
string bitconverter::readto0(byte* vc, int size, int off)
{
	string s = "";
	for (int i = off; i < size; i++)
	{
		if (vc[i] == 0) break;
		s += vc[i];
	}
	return s;
}