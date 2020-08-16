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
	//int pid = vmmgr::createProcess(lvmgr::formatPath("0:\\Neutrino\\bin\\conhost.lex"));
	//vmmgr::sendMessage(pid, bitconverter::toArray("0:\\Neutrino\\bin\\test.lex"));
	vmmgr::createProcess(lvmgr::formatPath("0:\\Neutrino\\bin\\testeffigy.lex"));
	//int pid = vmmgr::createProcess(lvmgr::formatPath("0:\\Neutrino\\bin\\error.lex"));
	//vmmgr::sendMessage(pid, bitconverter::toArray("The application Test has stopped working."));
	vmmgr::start();
}