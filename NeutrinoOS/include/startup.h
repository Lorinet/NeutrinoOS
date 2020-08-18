#pragma once
#include <iostream>
#include "vmmgr.h"
#include "lvmgr.h"
#include "iomgr.h"

static void NeutrinoStartup()
{
#if defined(__WIN32)
	lvmgr::initialize("C:");
#elif defined(__UNIX) || defined(__ESP32)
	lvmgr::initialize("/neutrino");
#endif
	IOManager::Initialize();
	if (file::fileExists(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\InitExecutable")))
	{
		string init = lvmgr::formatPath(file::readAllText(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\InitExecutable")));
		if (file::fileExists(init))
		{
			vmmgr::createProcess(init);
			vmmgr::start();
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