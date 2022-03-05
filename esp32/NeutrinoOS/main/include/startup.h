#pragma once
#include <iostream>
#include "vmmgr.h"
#include "lvmgr.h"
#include "config.h"
#include "nvm.h"
#include "kernlog.h"
#include "components.h"

static void NeutrinoStartup()
{
#if defined(__WIN32)
	lvmgr::initialize("C:");
#elif defined(__UNIX) || defined(__ESP32)
	lvmgr::initialize("/neutrino");
#endif
	klog("Init", "Registering system components and drivers...");
	components::registerComponents();
	klog("Init", "System components registered and available for use.");
	//if (config::keyExists("neutrino\\init"))
	//{
		//string init = lvmgr::formatPath(config::getValue("neutrino\\InitExecutable"));
		string init = lvmgr::formatPath("0:\\Neutrino\\bin\\test.lex");
		//string init1 = lvmgr::formatPath("0:\\Neutrino\\bin\\out.lex");
		if (file::fileExists(init))
		{
			vmmgr::start();
			for(int i = 0; i < 10; i++) vmmgr::createProcess(init);
			//vmmgr::createProcess(init);
			//vmmgr::schedulers[0]->processes[0]->processPriority = PRIORITY_HIGH;
			//vmmgr::kernelLoop();
			klog("Init", "System startup finished.");
			vmmgr::kernelLoopThread = thread(vmmgr::kernelLoop);
			vmmgr::kernelLoopThread.join();
		}
		else
		{
#if defined(COMPONENT_EFFIGY)
			Graphics::InitGraphicSystem();
#endif
			vmmgr::vmmerror("INIT_EXECUTABLE_NOT_FOUND");
		}
	//}
	//else
	//{
#if defined(COMPONENT_EFFIGY)
		Graphics::InitGraphicSystem();
#endif
		//vmmgr::vmmerror("INIT_CONFIGURATION_NOT_FOUND");
	//}
}
