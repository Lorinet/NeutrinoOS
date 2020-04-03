#include <iostream>
#include "vmmgr.h"
#include "lvmgr.h"
int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		lvmgr::initialize("/neutrino");
		vmmgr::createProcess(argv[1], true);
		vmmgr::start();
	}
	else
	{
		//cout << "Neutrino OS Loader v1.0" << endl << "Usage: neutrino <program.lex>" << endl;
		lvmgr::initialize("/neutrino");
		vmmgr::createProcess("/neutrino/Neutrino/bin/pins.lex", true);
		vmmgr::start();
	}
}
