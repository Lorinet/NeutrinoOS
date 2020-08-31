#include "util.h"
#ifdef __cplusplus
string util::replaceAll(string source, string from, string to)
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
string util::combinePath(string p1, string p2)
{
	string path = p1;
#ifdef __WIN32
	char delim = '\\';
#else
	char delim = '/';
#endif
	if(path[path.size() - 1] != delim && p2[0] != delim) path += delim;
	else if(path[path.size() - 1] == delim && p2[0] == delim) path = path.erase(path.size() - 1);
	path += p2;
	return path;
}
string util::pathGetParent(string path)
{
	string c = path;
	for(int i = path.size() - 1; i >= 0; i--)
	{
		c = c.erase(i);
		if(path[i] == '\\') break;
	}
	return c;
}
string util::pathGetLast(string path)
{
	string c = "";
	for(int i = path.size() - 1; i >= 0; i--)
	{
		if(path[i] == '\\') break;
		c = path[i] + c;
	}
	return c;
}
#endif