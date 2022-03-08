#include "scheduler.h"
#include "nvm.h"
#include "input.h"
#include "memorystats.h"
#include "kernlog.h"
#include "components.h"

scheduler::scheduler()
{
	running = false;
	processes = Array<nvm*>();
	eventSystem = new events(this);
}

scheduler::scheduler(scheduler& other)
{
	running = other.running;
	processes = other.processes;
	eventSystem = new events(this);
}

scheduler& scheduler::operator=(scheduler& other)
{
	running = other.running;
	processes = other.processes;
	eventSystem = new events(this);
	return *this;
}

scheduler::~scheduler()
{
	delete eventSystem;
}

void scheduler::start()
{
	if (run.joinable()) run.detach();
	run = thread([this]() { runScheduler(); });
}

void scheduler::suspend()
{
	running = false;
}

void scheduler::runScheduler()
{
	int pi = 0;
	int ii = 0;
	nvm* ehu = NULL;
	running = true;
	while (running)
	{
		lock_guard<mutex> lock(schedulerMutex);
		pi = 0;
		while (pi < processes.size)
		{
			if (pi == -1) break;
			if (processes[pi] != NULL && !processes[pi]->suspended)
			{
				ehu = processes[pi];
				if (!ehu->running)
				{
					removeProcessEx(pi);
					vmmgr::shutdown();
					pi--;
					continue;
				}
				ii = 0;
				input::inputLoop(ehu);
				if (ehu->messages.size > 0) ehu->awaitmsg = false;
				ehu->cycle();
			}
			if (processes[pi]->eventsenabled) processes[pi]->processEvents();
			pi++;
		}
		eventSystem->eventLoop();
		if (pi == 0)
		{
			running = false;
			return;
		}
	}
}

bool scheduler::checkAvailablePID(int pid)
{
	lock_guard<mutex> lock(schedulerMutex);
	for (int i = 0; i < processes.size; i++)
	{
		if (processes[i]->processid == pid) return false;
	}
	return true;
}

void scheduler::addProcess(nvm* process)
{
	lock_guard<mutex> lock(schedulerMutex);
	if (running)
	{
		suspend();
		run.join();
	}
	processes.add(process);
	start();
}

nvm* scheduler::getProcess(int pid)
{
	lock_guard<mutex> lock(schedulerMutex);
	for (int i = 0; i < processes.size; i++)
	{
		if (processes[i]->processid == pid) return processes[i];
	}
	return NULL;
}

void scheduler::removeProcess(int pid)
{
	lock_guard<mutex> lock(schedulerMutex);
	for (int i = 0; i < processes.size; i++)
	{
		if (processes[i]->processid == pid)
		{
			removeProcessEx(i);
			return;
		}
	}
}

void scheduler::sendMessage(int pid, Array<byte> msg)
{
	lock_guard<mutex> lock(schedulerMutex);
	if (running)
	{
		suspend();
		run.join();
	}
	getProcess(pid)->messages.add(msg);
	start();
}

void scheduler::sendTerminalInput(int pid, Array<byte> msg)
{
	lock_guard<mutex> lock(schedulerMutex);
	if (running)
	{
		suspend();
		run.join();
	}
	nvm* n = getProcess(pid);
	if (n->interm->type == vtype::MessageIn)
	{
		n->interm->registerInputData(msg);
	}
	start();
}

void scheduler::removeProcessEx(int index)
{
	vmmgr::processes.remove(processes[index]->processid);
	delete processes[index];
	processes.removeAt(index);
}