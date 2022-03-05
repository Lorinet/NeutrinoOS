#include "config.h"

string config::configDirectory = "0:\\Neutrino\\cfg\\";

bool config::keyExists(string key)
{
	return file::fileExists(lvmgr::formatPath(util::combinePath(configDirectory, key)));
}

string config::getValue(string key)
{
	if (keyExists(key)) return util::replaceAll(file::readAllText(lvmgr::formatPath(util::combinePath(configDirectory, key))), "\r\n", "\n");
	else return "";
}

int config::getValueInt(string key)
{
	if (!keyExists(key)) return -1;
	else
	{
		string val = file::readAllText(lvmgr::formatPath(util::combinePath(configDirectory, key)));
		for (int i = 0; i < val.size(); i++)
		{
			if (!isdigit(val[i])) return 0;
		}
		return stoi(val);
	}
}

void config::setValue(string key, string value)
{
	if (!keyExists(key)) file::createFile(lvmgr::formatPath(util::combinePath(configDirectory, key)));
	file::writeAllText(lvmgr::formatPath(util::combinePath(configDirectory, key)), value);
}

void config::setValue(string key, int value)
{
	setValue(key, to_string(value));
}