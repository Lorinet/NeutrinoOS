#include "scheduler.h"
#include "nvm.h"
#include "input.h"
#include "memorystats.h"
#include "kernlog.h"
#include "components.h"

scheduler::scheduler()
{
	setRunning(false);
	lock_guard<mutex> safeLock(eventsMutex);
	eventSystem = new events(this);
}

void scheduler::start()
{
	if (run.joinable()) run.join();
	run = thread(&scheduler::runScheduler, this);
}

void scheduler::suspend()
{
	setRunning(false);
}

void scheduler::runScheduler()
{
	int pi = 0;
	int ii = 0;
	int numproc = processCount();
	nvm* ehu = NULL;
	setRunning(true);
	while (getRunning())
	{
		pi = 0;
		while (pi < numproc)
		{
			lock_guard<mutex> lock(processesMutex);
			ehu = processes[pi];
			if (!ehu->running)
			{
				vmmgr::releaseProcessID(ehu->processid);
				delete processes[pi];
				processes.removeAt(pi);
				numproc = processes.size;
				continue;
			}
			ii = 0;
			input::inputLoop(ehu);
			if (ehu->messages.size > 0) ehu->awaitmsg = false;
			ehu->cycle();
			if (processes[pi]->eventsenabled) processes[pi]->processEvents();
			pi++;
		}
		eventSystem->eventLoop();
		if (pi <= 0)
		{
			setRunning(false);
			return;
		}
	}
}

void scheduler::addProcess(nvm* process)
{
	{
		lock_guard<mutex> lock(processesMutex);
		processes.add(process);
	}
	start();
}

nvm* scheduler::getProcess(int pid)
{
	lock_guard<mutex> lock(processesMutex);
	for (int i = 0; i < processes.size; i++)
	{
		if (processes[i]->processid == pid) return processes[i];
	}
	return NULL;
}

void scheduler::removeProcess(int pid)
{
	lock_guard<mutex> lock(processesMutex);
	for (int i = 0; i < processes.size; i++)
	{
		if (processes[i]->processid == pid)
		{
			delete processes[i];
			processes.removeAt(i);
			vmmgr::releaseProcessID(pid);
			return;
		}
	}
}

void scheduler::sendMessage(int pid, Array<byte> msg)
{
	lock_guard<mutex> lock(processesMutex);
	if (getRunning())
	{
		setRunning(false);
		run.join();
	}
	getProcess(pid)->messages.add(msg);
	start();
}

void scheduler::sendTerminalInput(int pid, Array<byte> msg)
{
	lock_guard<mutex> lock(processesMutex);
	if (getRunning())
	{
		setRunning(false);
		run.join();
	}
	nvm* n = getProcess(pid);
	if (n->interm->type == vtype::MessageIn)
	{
		n->interm->registerInputData(msg);
	}
	start();
}

bool scheduler::getRunning()
{
	lock_guard<mutex> lock(runningMutex);
	return running;
}

void scheduler::setRunning(bool r)
{
	lock_guard<mutex> lock(runningMutex);
	running = r;
}

int scheduler::processCount()
{
	lock_guard<mutex> safeLock(processesMutex);
	return processes.size;
}