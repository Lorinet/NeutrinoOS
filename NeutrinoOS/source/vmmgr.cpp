#include "vmmgr.h"
#include "nvm.h"
#include "vt.h"
#include "disassembler.h"
#include "lvmgr.h"
#include "filesystem.h"
#include "event.h"
#include "syscall.h"
#include "nvm.h"
#include "kernlog.h"
#include "components.h"
#include "memorystats.h"
bool vmmgr::running = false;
bool vmmgr::dozing = false;
int vmmgr::maxCpuFreq = 0;
int vmmgr::minCpuFreq = 0;

Array<scheduler*> vmmgr::schedulers;
IntMap<nvm*> vmmgr::processes;
mutex vmmgr::kernelMutex;
thread vmmgr::kernelLoopThread;
unique_lock<mutex> vmmgr::kernelLock;

void vmmgr::start()
{
	klog("VirtualMachineManager", "Starting...");
	running = true;
	dozing = false;
	int dog = 0;
#if defined(__ESP32)
	esp_task_wdt_init(3, false);
	klog("ESP32", "Disabled idle task watchdog");
#elif defined(__UNIX)
	if (file::fileExists(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMaxFreq")) && file::fileExists(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMinFreq")))
	{
		minCpuFreq = atoi(file::readAllText(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMinFreq")).c_str());
		maxCpuFreq = atoi(file::readAllText(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMaxFreq")).c_str());
	}
#elif defined(__WIN32)
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif
	klog("VirtualMachineManager", "Starting schedulers...");
	kernelLock = unique_lock<mutex>(kernelMutex, defer_lock);
	schedulers = Array<scheduler*>();
	for (int thr = 0; thr < MAX_THREADS_ALLOWED; thr++)
	{
		schedulers.add(new scheduler());
	}
	for (int i = 0; i < schedulers.size; i++)
	{
		schedulers[i]->start();
	}
	klog("VirtualMachineManager", to_string(schedulers.size) + " schedulers available.");
}

int vmmgr::createProcess(string file, bool start)
{
	int procid = 0;
	#ifndef __ESP32
	try
	{
	#endif
		scheduler* sch = schedulers[0];
		for (int i = 0; i < schedulers.size; i++)
		{
			if (schedulers[i]->processes.size < sch->processes.size) sch = schedulers[i];
		}
		int progsize;
		byte* pcode = file::readAllBytes(file, &progsize);
		nvm* n = new nvm(disassembler::disassembleExecutable(pcode, progsize));
		delete[] pcode;
		int pid = 0;
		bool done = true;
		while (!done)
		{
			if (!processes.find(pid))
			{
				done = true;
				break;
			}
			pid++;
		}
		if (start) n->start(pid, file);
		else
		{
			n->fileName = file;
			n->processid = pid;
		}
		n->processPriority = PRIORITY_LOW;
		processes.add(pid, n);
		sch->addProcess(n);
		return pid;
	#ifndef __ESP32
	}
	catch (string ex)
	{
		vmmerror(ex);
	}
	#endif
	return -1;
}

int vmmgr::createProcessEx(string file, vt in, vt out)
{
	int pid = createProcess(file);
	nvm* cp = getProcess(pid);
	cp->setTerminals(in, out);
	cp->start();
	return -1;
}

void vmmgr::terminateProcess(int pid)
{
	if (getProcess(pid) != NULL)
	{
		getProcess(pid)->running = false;
#if defined(COMPONENT_TIWAZ)
		Array<int> deathRow = Array<int>();
		for (map<int, View*>::iterator it = ViewManager::views.begin(); it != ViewManager::views.end(); it++)
		{
			if (ViewManager::views[it->first]->parentProcess == pid)
			{
				deathRow.add(it->first);
			}
		}
		for (int i = 0; i < deathRow.size; i++)
		{
			ViewManager::CloseView(deathRow[i]);
		}
#endif
	}
}

nvm* vmmgr::getProcess(int pid)
{
	if (processes.find(pid)) return processes[pid];
	return NULL;
}

void vmmgr::sendMessage(int pid, Array<byte> msg)
{
	int psi = -1;
	for (int i = 0; i < schedulers.size; i++)
	{
		for (int j = 0; j < schedulers[i]->processes.size; j++)
		{
			if (schedulers[i]->processes[j]->processid == pid)
			{
				psi = i;
				break;
			}
		}
	}
	if (psi != -1) schedulers[psi]->sendMessage(pid, msg);
}

void vmmgr::sendInput(int pid, Array<byte> msg)
{
	int psi = -1;
	for (int i = 0; i < schedulers.size; i++)
	{
		for (int j = 0; j < schedulers[i]->processes.size; j++)
		{
			if (schedulers[i]->processes[j]->processid == pid)
			{
				psi = i;
				break;
			}
		}
	}
	if (psi != -1) schedulers[psi]->sendTerminalInput(pid, msg);
}

void vmmgr::enterCriticalSection()
{
	kernelLock.lock();
	//klog("VirtualMachineManager", "Enter Critical Section");
}

void vmmgr::leaveCriticalSection()
{
	kernelLock.unlock();
	//klog("VirtualMachineManager", "Leave Critical Section");
}

void vmmgr::doze(bool d)
{
	dozing = d;
#ifdef __UNIX
	if (minCpuFreq == 0 || maxCpuFreq == 0) return;
	//if(d) system("cgset -r cpu.cfs_quota_us=100000 cpulimit");
	//else system("cgset -r cpu.cfs_quota_us=1000000 cpulimit");
	if (d) system(("cpufreq-set -r --max " + to_string(minCpuFreq) + "MHz").c_str());
	else system(("cpufreq-set -r --max " + to_string(maxCpuFreq) + "MHz").c_str());
#endif
}

void vmmgr::vmmerror(string error)
{
#if defined(COMPONENT_EFFIGY)
	WindowManager::ErrorScreen(error);
#else
	cout << "CRITICAL ERROR: " << error << endl;
#endif
	while (true);
}

void vmmgr::vmmerror(string error, int procid)
{
	nvm* n = getProcess(procid);
	if (n != NULL)
	{
		n->running = false;
#ifdef __WIN32
		string appname = util::getLast(n->fileName, '\\');
#else
		string appname = util::getLast(n->fileName, '/');
#endif
		if (file::fileExists(lvmgr::formatPath("0:\\Neutrino\\bin\\error.lex")))
		{
			int pid = createProcess(lvmgr::formatPath("0:\\Neutrino\\bin\\error.lex"));
			sendMessage(pid, bitconverter::toArray("Application " + appname + " stopped working\\: " + util::replaceAll(error, ":", "\\:")));
		}
		cout << "ERROR: Application " << appname << " (" << procid << ") stopped working: " << error << endl;
		terminateProcess(procid);
	}
	else
	{
		cout << "ERROR: " << error << endl;
	}
}
bool vmmgr::inputRequested(int pid)
{
	nvm* n = getProcess(pid);
	if (n != NULL)
	{
		return n->awaitin;
	}
	return false;
}
void vmmgr::kernelLoop()
{
	while (running)
	{

	}
}