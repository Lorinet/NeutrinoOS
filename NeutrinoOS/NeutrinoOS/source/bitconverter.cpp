#include "bitconverter.h"
int bitconverter::toint32(byte* vc, int index)
{
	return vc[index + 3] << 24 | vc[index + 2] << 16 | vc[index + 1] << 8 | vc[index];
}
int bitconverter::toint32(vector<byte> vc, int index)
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
string bitconverter::tostring(vector<byte> vc)
{
	string s = "";
	for (byte b : vc)
	{
		s += b;
	}
	return s;
}
string bitconverter::tostring(vector<byte> vc, int off)
{
	string s = "";
	for (int i = off; i < vc.size(); i++)
	{
		s += vc[i];
	}
	return s;
}
string bitconverter::readto0(vector<byte> vc, int off)
{
	string s = "";
	for (int i = off; i < vc.size(); i++)
	{
		if (vc[i] == 0) break;
		s += vc[i];
	}
	return s;
}
vector<byte> bitconverter::subvector(vector<byte> vc, int off)
{
	vector<byte> v;
	for (int i = off; i < vc.size(); i++)
	{
		v.push_back(vc[i]);
	}
	return v;
}