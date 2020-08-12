#include "util.h"
#ifdef __cplusplus
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
string util::getLast(string str, char del)
{
	stringstream ss = stringstream(str);
	string sec = "";
	while(getline(ss, sec, del));
	return sec;
}
bool util::startsWith(string s, string sta)
{
	return s.rfind(sta, 0) == 0;
}
#endif