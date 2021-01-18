#include "vmmgr.h"
#include "nvm.h"
#include "vt.h"
bool vmmgr::running = false;
bool vmmgr::dozing = false;
int vmmgr::maxCpuFreq = 0;
int vmmgr::minCpuFreq = 0;
#if defined(VMM_LEGACY_SCHEDULER)
map<int, nvm*> vmmgr::processes;
vector<int> vmmgr::procidx;
void vmmgr::start()
{
#if defined(COMPONENT_TIWAZ)
	ViewManager::Initialize();
	inputmgr::initialize();
#elif defined(COMPONENT_EFFIGY)
	WindowManager::Initialize();
#endif
#if defined(FEATURE_SERIAL) || defined(FEATURE_GPIO)
	IOManager::Initialize();
#endif
	running = true;
	dozing = false;
	int dog = 0;
#if defined(__ESP32)
	esp_task_wdt_init(3, false);
#elif defined(__UNIX)
	if (file::fileExists(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMaxFreq")) && file::fileExists(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMinFreq")))
	{
		minCpuFreq = atoi(file::readAllText(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMinFreq")).c_str());
		maxCpuFreq = atoi(file::readAllText(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMaxFreq")).c_str());
	}
#endif
	while (running)
	{
		if (dog >= 100)
		{
#ifdef __ESP32
			esp_task_wdt_reset();
#endif
#if defined(COMPONENT_EFFIGY)
			bool dozeNow = true;
			for (int i = 0; i < procidx.size(); i++)
			{
				if (!processes[procidx[i]]->suspended && !processes[procidx[i]]->awaitmsg && !processes[procidx[i]]->awaitin && processes[procidx[i]]->waitForProcInput == -1)
				{
					dozeNow = false;
					break;
				}
			}
			if (dozeNow && !dozing)
			{
				doze(true);
			}
			else if (!dozeNow && dozing)
			{
				doze(false);
			}
#endif
			dog = 0;
		}
		else
		{
			dog++;
		}
		stepAll();
	}
}
void vmmgr::stepAll()
{
	for (unsigned int i = 0; i < procidx.size(); i++)
	{
		if (!processes[procidx[i]]->suspended)
		{
			input::inputLoop(processes[procidx[i]]);
			if (processes[procidx[i]]->messages.size > 0) processes[procidx[i]]->awaitmsg = false;
			processes[procidx[i]]->cycle();
		}
		if (processes[procidx[i]]->running == false)
		{
			terminateProcess(procidx[i]);
			i -= 1;
		}
	}
	events::eventLoop();
#if defined(COMPONENT_TIWAZ)
	inputmgr::poll();
#elif defined(COMPONENT_EFFIGY)
	WindowManager::Update();
#endif
}
int vmmgr::createProcess(string file)
{
	int procid = 0;
	//try
	//{
	while (processes.count(procid))
	{
		procid += 1;
	}
	int progsize;
	byte* pcode = file::readAllBytes(file, &progsize);
	processes.insert({ procid, new nvm(disassembler::disassembleExecutable(pcode, progsize)) });
	processes[procid]->start(procid, file);
	procidx.push_back(procid);
	return procid;
	//}
	//catch (string ex)
	//{
	//	vmmerror(ex);
	//}
	return -1;
}
int vmmgr::createProcessEx(string file, vt in, vt out)
{
	int procid = 0;
	//try
	//{
	while (processes.count(procid))
	{
		procid += 1;
	}
	int progsize;
	byte* pcode = file::readAllBytes(file, &progsize);
	processes.insert({ procid, new nvm(disassembler::disassembleExecutable(pcode, progsize)) });
	*processes[procid]->interm = in;
	*processes[procid]->outterm = out;
	processes[procid]->start(procid, file);
	processes[procid]->suspended = true;
	procidx.push_back(procid);
	return procid;
	//}
	//catch (string ex)
	//{
	//	vmmerror(ex);
	//}
	return -1;
}
void vmmgr::terminateProcess(int pid)
{
	if (processes.count(pid))
	{
		processes.erase(pid);
		procidx.erase(find(procidx.begin(), procidx.end(), pid));
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
void vmmgr::sendMessage(int pid, Array<byte> msg)
{
	if (processes.count(pid))
	{
		processes[pid]->messages.push(msg);
	}
}
void vmmgr::sendInput(int pid, Array<byte> msg)
{
	if (processes.count(pid))
	{
		if (processes[pid]->interm->type == vtype::MessageIn)
		{
			processes[pid]->interm->registerInputData(msg);
		}
	}
}
void vmmgr::reload()
{
	suspend();
	processes.clear();
	start();
}
void vmmgr::suspend()
{
	running = false;
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
	if (processes.find(procid) != processes.end())
	{
		processes[procid]->running = false;
#ifdef __WIN32
		string appname = util::getLast(processes[procid]->fileName, '\\');
#else
		string appname = util::getLast(processes[procid]->fileName, '/');
#endif
		if (file::fileExists(lvmgr::formatPath("0:\\Neutrino\\bin\\error.lex")))
		{
			int pid = createProcess(lvmgr::formatPath("0:\\Neutrino\\bin\\error.lex"));
			sendMessage(pid, bitconverter::toArray("Application " + appname + " stopped working\\: " + util::replaceAll(error, ":", "\\:")));
		}
		cout << "ERROR: Application " << appname << " (" << procid << ") stopped working: " << error << endl;
	}
	else
	{
		cout << "ERROR: " << error << endl;
	}
}
bool vmmgr::inputRequested(int pid)
{
	if (processes.count(pid))
	{
		return processes[pid]->awaitin;
	}
	return false;
}
#else
Array<scheduler*> vmmgr::schedulers;
thread vmmgr::eventloop;
void vmmgr::start()
{
#if defined(COMPONENT_TIWAZ)
	ViewManager::Initialize();
	inputmgr::initialize();
#elif defined(COMPONENT_EFFIGY)
	WindowManager::Initialize();
#endif
#if defined(FEATURE_SERIAL) || defined(FEATURE_GPIO)
	IOManager::Initialize();
#endif
	running = true;
	dozing = false;
	int dog = 0;
#if defined(__ESP32)
	esp_task_wdt_init(3, false);
#elif defined(__UNIX)
	if (file::fileExists(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMaxFreq")) && file::fileExists(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMinFreq")))
	{
		minCpuFreq = atoi(file::readAllText(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMinFreq")).c_str());
		maxCpuFreq = atoi(file::readAllText(lvmgr::formatPath("0:\\Neutrino\\cfg\\neutrino\\CPUMaxFreq")).c_str());
	}
#elif defined(__WIN32)
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif
	schedulers = Array<scheduler*>();
	for (int thr = 0; thr < MAX_THREADS_ALLOWED; thr++)
	{
		schedulers.add(new scheduler());
	}
	for (int i = 0; i < schedulers.size; i++)
	{
		schedulers[i]->start();
	}
}

void vmmgr::kernelLoop()
{
	while (running)
	{
		this_thread::sleep_for(chrono::milliseconds(100));
		//events::eventLoop();
#if defined(COMPONENT_TIWAZ)
		inputmgr::poll();
#elif defined(COMPONENT_EFFIGY)
		WindowManager::Update();
#endif
	}
}

int vmmgr::createProcess(string file, bool start)
{
	int procid = 0;
	try
	{
		scheduler* sch = schedulers[0];
		for (int i = 0; i < schedulers.size; i++)
		{
			if (schedulers[i]->processes.size < sch->processes.size) sch = schedulers[i];
		}
		int progsize;
		byte* pcode = file::readAllBytes(file, &progsize);
		nvm* n = new nvm(disassembler::disassembleExecutable(pcode, progsize));
		int pid = 0;
		bool done = true;
		while (!done)
		{
			done = true;
			for (int i = 0; i < schedulers.size; i++)
			{
				if (!schedulers[i]->checkAvailablePID(pid))
				{
					done = false;
					pid++;
					break;
				}
			}
		}
		if (start) n->start(pid, file);
		else
		{
			n->fileName = file;
			n->processid = pid;
		}
		sch->addProcess(n);
		return pid;
	}
	catch (string ex)
	{
		vmmerror(ex);
	}
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
	for (int i = 0; i < schedulers.size; i++)
	{
		for (int j = 0; j < schedulers[i]->processes.size; j++)
		{
			if (schedulers[i]->processes[j]->processid == pid) return schedulers[i]->processes[j];
		}
	}
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

void vmmgr::reload()
{
	running = false;
	eventloop.join();
	for (int i = 0; i < schedulers.size; i++)
	{
		schedulers[i]->suspend();
		schedulers[i]->run.join();
		for (int j = 0; j < schedulers[i]->processes.size; j++)
		{
			delete schedulers[i]->processes[j];
			schedulers[i]->processes.removeAt(j);
		}
		delete schedulers[i];
		schedulers.removeAt(i);
	}
	start();
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
#endif