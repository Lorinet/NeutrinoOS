#include "scheduler.h"
#include "nvm.h"

scheduler::scheduler()
{
	running = false;
	processes = Array<nvm*>();
}

scheduler::scheduler(scheduler& other)
{
	running = other.running;
	processes = other.processes;
}

scheduler& scheduler::operator=(scheduler& other)
{
	running = other.running;
	processes = other.processes;
	return *this;
}

scheduler::~scheduler()
{
	//processes.~Array();
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
					pi--;
					continue;
				}
				ii = 0;
				input::inputLoop(ehu);
				if (ehu->messages.size > 0) ehu->awaitmsg = false;
				while (ii < ehu->processPriority)
				{
					ehu->cycle();
					ii++;
				}
			}
			pi++;
		}
		if (pi == 0)
		{
			running = false;
			return;
		}
	}
}

bool scheduler::checkAvailablePID(int pid)
{
	for (int i = 0; i < processes.size; i++)
	{
		if (processes[i]->processid == pid) return false;
	}
	return true;
}

void scheduler::addProcess(nvm* process)
{
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
	for (int i = 0; i < processes.size; i++)
	{
		if (processes[i]->processid == pid) return processes[i];
	}
}

void scheduler::removeProcess(int pid)
{
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
	cout << "Time of execution: " << ntime::getMillis() - processes[index]->millis << endl;
	delete processes[index];
	processes.removeAt(index);
}