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

atomic<bool> vmmgr::running;
bool vmmgr::dozing = false;
int vmmgr::maxCpuFreq = 0;
int vmmgr::minCpuFreq = 0;
int vmmgr::numThreads = 1;
scheduler* vmmgr::schedulers;
mutex vmmgr::kernelMutex;
mutex vmmgr::processIdsMutex;
IntMap<int> vmmgr::processIds;
thread vmmgr::kernelLoopThread;

void vmmgr::start()
{
	lock_guard<mutex> lock(kernelMutex);
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
	schedulers = new scheduler[numThreads];
	//kernelLock = unique_lock<mutex>(kernelMutex, defer_lock);
	for (int i = 0; i < numThreads; i++)
	{
		schedulers[i].start();
	}
	klog("VirtualMachineManager", to_string(numThreads) + " schedulers available.");
}

void vmmgr::shutdown()
{
	running = false;
}

int vmmgr::createProcess(string file, bool start)
{
	int procid = 0;
	int sid = 0;
	for (int i = 0; i < numThreads; i++)
	{
		if (schedulers[i].processCount() < schedulers[sid].processCount()) sid = i;
	}
	//lock_guard<mutex> safeLock(kernelMutex);
	int progsize;
	byte* pcode = file::readAllBytes(file, &progsize);
	nvm* n = new nvm(disassembler::disassembleExecutable(pcode, progsize));
	delete[] pcode;
	int pid = 0;
	{
		lock_guard<mutex> lock(processIdsMutex);
		while(true)
		{
			if(!processIds.find(pid) || processIds[pid] == -1) break;
			pid++;
		}
		processIds.set(pid, sid);
	}
	bool done = true;
	if (start) n->start(pid, file);
	else
	{
		n->fileName = file;
		n->setpid(pid);
	}
	n->processPriority = PRIORITY_LOW;
	schedulers[sid].addProcess(n);
	return pid;
}

int vmmgr::createProcessEx(string file, vt in, vt out)
{
	int pid = createProcess(file);
	nvm* cp = getProcess(pid);
	lock_guard<mutex> lock(kernelMutex);
	cp->setTerminals(in, out);
	cp->start();
	return -1;
}

void vmmgr::terminateProcess(int pid)
{
	lock_guard<mutex> lock(kernelMutex);
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
	lock_guard<mutex> kernelLock(kernelMutex);
	lock_guard<mutex> lock(processIdsMutex);
	return schedulers[processIds[pid]].getProcess(pid);
}

void vmmgr::sendMessage(int pid, Array<byte> msg)
{
	lock_guard<mutex> lock(kernelMutex);
	/*int psi = -1;
	for (int i = 0; i < MAX_THREADS_ALLOWED; i++)
	{
		for (int j = 0; j < schedulers[i].processes.size; j++)
		{
			if (schedulers[i].processes[j]->processid == pid)
			{
				psi = i;
				break;
			}
		}
	}
	if (psi != -1) schedulers[psi].sendMessage(pid, msg);*/
}

void vmmgr::sendInput(int pid, Array<byte> msg)
{
	lock_guard<mutex> lock(kernelMutex);
	/*int psi = -1;
	for (int i = 0; i < MAX_THREADS_ALLOWED; i++)
	{
		for (int j = 0; j < schedulers[i].processes.size; j++)
		{
			if (schedulers[i].processes[j]->processid == pid)
			{
				psi = i;
				break;
			}
		}
	}
	if (psi != -1) schedulers[psi].sendTerminalInput(pid, msg);*/
}

void vmmgr::enterCriticalSection()
{
	//kernelLock.lock();
	//klog("VirtualMachineManager", "Enter Critical Section");
}

void vmmgr::leaveCriticalSection()
{
	//kernelLock.unlock();
	//klog("VirtualMachineManager", "Leave Critical Section");
}

void vmmgr::doze(bool d)
{
	dozing = d;
#ifdef __UNIX
	if (minCpuFreq == 0 || maxCpuFreq == 0) return;
	//if(d) system("cgset -r cpu.cfs_quota_us=100000 cpulimit");
	//else system("cgset -r cpu.cfs_quota_us=1000000 cpulimit");
	//if (d) system(("cpufreq-set -r --max " + to_string(minCpuFreq) + "MHz").c_str());
	//else system(("cpufreq-set -r --max " + to_string(maxCpuFreq) + "MHz").c_str());
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

void vmmgr::releaseProcessID(int pid)
{
	lock_guard<mutex> lock(processIdsMutex);
	processIds.set(pid, -1);
}

void vmmgr::kernelLoop()
{
	while (true)
	{
		//lock_guard<mutex> safeLock(kernelMutex);
		if(!running) break;
	}
	for (int i = 0; i < numThreads; i++)
	{
		schedulers[i].suspend();
	}
	for (int i = 0; i < numThreads; i++)
	{
		if(schedulers[i].run.joinable()) schedulers[i].run.join();
	}
}
