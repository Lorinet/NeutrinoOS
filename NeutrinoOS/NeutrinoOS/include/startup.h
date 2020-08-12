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
	//int pid = vmmgr::createProcess(lvmgr::formatPath("0:\\Neutrino\\bin\\conhost.lex"), true);
	//vmmgr::sendMessage(pid, bitconverter::toArray("0:\\Neutrino\\bin\\hello.lex"));
	vmmgr::createProcess(lvmgr::formatPath("0:\\Neutrino\\bin\\test.lex"));
	vmmgr::start();
}