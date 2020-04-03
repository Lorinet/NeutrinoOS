#include "coreos.h"
void neutrinoos::startup(string startupProcess)
{
	//textscreen::println(aboutNeutrino);
	vmmgr::createProcess(startupProcess, true);
	vmmgr::start();
}