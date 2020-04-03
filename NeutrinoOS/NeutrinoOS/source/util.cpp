#include "util.h"
void util::remove(vector<byte> vc, int index)
{
	vc.erase(vc.begin() + index);
}
vector<byte> util::initvect(int capacity)
{
	vector<byte> v;
	for (int i = 0; i < capacity; i++)
	{
		v.push_back(0);
	}
	return v;
}
string util::replaceAll(string& source, const string& from, const string& to)
{
	string newString;
	newString.reserve(source.length());
	string::size_type lastPos = 0;
	string::size_type findPos;
	while (string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}
	newString += source.substr(lastPos);
	//source.swap(newString);
	return newString;
}
bool util::contains(deque<int> d, int i)
{
	for (int j = 0; j < d.size(); j++)
	{
		if (d[j] == i) return true;
	}
	return false;
}
bool util::startsWith(string s, string sta)
{
	return s.rfind(sta, 0) == 0;
}