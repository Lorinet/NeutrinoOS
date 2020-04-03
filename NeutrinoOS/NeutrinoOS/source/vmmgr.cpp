#include "vmmgr.h"
#include "nvm.h"
#include "vt.h"
bool vmmgr::running = false;
map<int, nvm> vmmgr::processes;
int vmmgr::activeProcess;
int vmmgr::prevActiveProcess;
vector<int> vmmgr::procidx;
void vmmgr::start()
{
#if defined(COMPONENT_TIWAZ)
	ViewManager::Initialize();
	inputmgr::initialize();
#endif
	running = true;
	while (running)
	{
		stepAll();
	}
}
void vmmgr::stepAll()
{
	for (unsigned int i = 0; i < procidx.size(); i++)
	{
		if (!processes[procidx[i]].suspended)
		{
			input::inputLoop(&processes[procidx[i]]);
#if defined(COMPONENT_TIWAZ)
			inputmgr::poll();
#endif
			processes[procidx[i]].cycle();
		}
		if (processes[procidx[i]].running == false)
		{
			terminateProcess(procidx[i]);
			i -= 1;
		}
	}
	events::eventLoop();
}
int vmmgr::startProgram(string path, bool active)
{
	return createProcess(path, active);
}
int vmmgr::createProcess(string file, bool active)
{
	int procid = 0;
	try
	{
		while (processes.count(procid))
		{
			procid += 1;
		}
		processes.insert({ procid, nvm(disassembler::disassembleExecutable(file::readAllBytes(file))) });
		if (active)
		{
			prevActiveProcess = activeProcess;
			activeProcess = procid;
		}
		processes[procid].start(procid);
		procidx.push_back(procid);
		return procid;
	}
	catch (string ex)
	{
		vmmerror(ex);
	}
	return -1;
}
int vmmgr::createProcessEx(string file, vt in, vt out)
{
	int procid = 0;
	try
	{
		while (processes.count(procid))
		{
			procid += 1;
		}
		processes.insert({ procid, nvm(disassembler::disassembleExecutable(file::readAllBytes(file))) });
		*processes[procid].interm = in;
		*processes[procid].outterm = out;
		processes[procid].start(procid);
		processes[procid].suspended = true;
		procidx.push_back(procid);
		return procid;
	}
	catch (string ex)
	{
		vmmerror(ex);
	}
	return -1;
}
void vmmgr::terminateProcess(int pid)
{
	if (processes.count(pid))
	{
		processes.erase(pid);
		if (pid == activeProcess)
		{
			activeProcess = prevActiveProcess;
		}
		procidx.erase(remove(procidx.begin(), procidx.end(), pid));
	}
}
void vmmgr::sendMessage(int pid, vector<byte> msg)
{
	if (processes.count(pid))
	{
		processes[pid].messages.push(msg);
		processes[pid].awaitmsg = false;
	}
}
void vmmgr::sendInput(int pid, vector<byte> msg)
{
	if (processes.count(pid))
	{
		if (processes[pid].interm->type == vtype::MessageIn)
		{
			processes[pid].interm->registerInputData(msg);
		}
	}
}
void vmmgr::reload()
{
	suspend();
	//processes = map<int, nvm>();
	start();
}
void vmmgr::suspend()
{
	running = false;
}
void vmmgr::vmmerror(string error)
{
	cout << error << endl;
}
void vmmgr::vmmerror(string error, int procid)
{
	cout << error << procid << endl;
}
bool vmmgr::inputRequested(int pid)
{
	if (processes.count(pid))
	{
		return processes[pid].awaitin;
	}
}