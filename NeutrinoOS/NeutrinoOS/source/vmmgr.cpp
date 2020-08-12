#include "vmmgr.h"
#include "nvm.h"
#include "vt.h"
bool vmmgr::running = false;
map<int, nvm*> vmmgr::processes;
vector<int> vmmgr::procidx;
void vmmgr::start()
{
#if defined(COMPONENT_TIWAZ)
	ViewManager::Initialize();
	inputmgr::initialize();
#endif
#if defined(FEATURE_SERIAL) || defined(FEATURE_GPIO)
	IOManager::Initialize();
#endif
	running = true;
	#ifdef __ESP32
	int dog = 0;
	esp_task_wdt_init(3, false);
	#endif
	while (running)
	{
		#ifdef __ESP32
		if(dog >= 100) 
		{
			esp_task_wdt_reset();
			dog = 0;
		}
		else
		{
			dog++;
		}
		#endif
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
		for(map<int, View *>::iterator it = ViewManager::views.begin(); it != ViewManager::views.end(); it++)
		{
			if(ViewManager::views[it->first]->parentProcess == pid)
			{
				deathRow.add(it->first);
			}
		}
		for(int i = 0; i < deathRow.size; i++)
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
void vmmgr::vmmerror(string error)
{
	cout << error << endl;
}
void vmmgr::vmmerror(string error, int procid)
{
	if(processes.find(procid) != processes.end())
	{
		processes[procid]->running = false;
		#ifdef __WIN32
		string appname = util::getLast(processes[procid]->fileName, '\\');
		#else
		string appname = util::getLast(processes[procid]->fileName, '/');
		#endif
		if(file::fileExists(lvmgr::formatPath("0:\\Neutrino\\bin\\error.lex")))
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