#include "lvmgr.h"
map<string, string> lvmgr::drives;
void lvmgr::initialize(string zer0)
{
	mount("0", zer0);
}
void lvmgr::mount(string letter, string folder)
{
	if (drives.find(letter) == drives.end())
	{
		drives.insert({ letter, folder });
	}
}
void lvmgr::unmount(string letter)
{
	drives.erase(letter);
}
vector<string> lvmgr::getDrives()
{
	vector<string> d;
	for (map<string, string>::iterator it = drives.begin(); it != drives.end(); it++)
	{
		d.push_back(it->first);
	}
	return d;
}
bool lvmgr::driveExists(string letter)
{
	return (drives.find(letter) != drives.end());
}
string lvmgr::formatPath(string path)
{
	string p = path;
	string drv = string(1, p[0]);  
	p.erase(0, 2);
	drv = drives[drv];
	p = drv + p;
#if defined(__UNIX)
	p = util::replaceAll(p, "\\", "/");
#endif
	return p;
}