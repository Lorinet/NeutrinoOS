#pragma once
#include <thread>
#include "containers.h"
#define PRIORITY_MIN 1
#define PRIORITY_LOW 8
#define PRIORITY_MEDIUM 16
#define PRIORITY_HIGH 32
#define PRIORITY_MAX 64
using namespace std;
class nvm;
class events;
class scheduler
{
public:
	Array<nvm*> processes;
	events* eventSystem;
	bool running;
	thread run;
	scheduler();
	scheduler(scheduler& other);
	~scheduler();
	scheduler& operator=(scheduler& other);
	void start();
	void suspend();
	void runScheduler();
	void addProcess(nvm* process);
	nvm* getProcess(int pid);
	void removeProcess(int pid);
	void removeProcessEx(int index);
	void sendMessage(int pid, Array<byte> msg);
	void sendTerminalInput(int pid, Array<byte> msg);
	bool checkAvailablePID(int pid);
};