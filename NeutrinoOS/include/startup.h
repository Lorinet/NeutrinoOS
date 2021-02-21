#pragma once
#include <iostream>
#include "vmmgr.h"
#include "lvmgr.h"
#include "config.h"
#include "iomgr.h"
#include "nvm.h"

static void NeutrinoStartup()
{
#if defined(__WIN32)
	lvmgr::initialize("C:");
#elif defined(__UNIX) || defined(__ESP32)
	lvmgr::initialize("/neutrino");
#endif
	IOManager::Initialize();
	if (config::keyExists("neutrino\\InitExecutable"))
	{
		string init = lvmgr::formatPath(config::getValue("neutrino\\InitExecutable"));
		if (file::fileExists(init))
		{
			vmmgr::start();
			for(int i = 0; i < 10000; i++) vmmgr::createProcess(init);
			vmmgr::schedulers[0]->processes[0]->processPriority = PRIORITY_HIGH;
			vmmgr::kernelLoop();
		}
		else
		{
#if defined(COMPONENT_EFFIGY)
			Graphics::InitGraphicSystem();
#endif
			vmmgr::vmmerror("INIT_EXECUTABLE_NOT_FOUND");
		}
	}
	else
	{
#if defined(COMPONENT_EFFIGY)
		Graphics::InitGraphicSystem();
#endif
		vmmgr::vmmerror("INIT_CONFIGURATION_NOT_FOUND");
	}
}