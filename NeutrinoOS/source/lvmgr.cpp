#include "lvmgr.h"
#include "kernlog.h"
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
		klog("LogicalVolumeManager", "Mounted " + folder + " as volume " + letter + ":");
	}
	else
	{
		kwarn("LogicalVolumeManager", "Volume " + letter + ": already mounted: " + drives[letter]);
	}
}
void lvmgr::unmount(string letter)
{
	drives.erase(letter);
	klog("LogicalVolumeManager", "Unmounted volume " + letter + ":");
}
Array<string> lvmgr::getDrives()
{
	Array<string> d;
	for (map<string, string>::iterator it = drives.begin(); it != drives.end(); it++)
	{
		d.push(it->first);
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
#if defined(__UNIX) || defined(__ESP32)
	p = util::replaceAll(p, "\\", "/");
#endif
	return p;
}