#pragma once
#include <thread>
#include <mutex>
#include "containers.h"
#define PRIORITY_MIN 1
#define PRIORITY_LOW 4
#define PRIORITY_MEDIUM 8
#define PRIORITY_HIGH 16
#define PRIORITY_MAX 64
using namespace std;
class nvm;
class events;
class scheduler
{
private:
	bool running;
	Array<nvm*> processes;
	mutex processesMutex;
	mutex runningMutex;
	mutex eventsMutex;
public:
	events* eventSystem;
	thread run;
	scheduler();
	void start();
	void suspend();
	void runScheduler();
	void addProcess(nvm* process);
	nvm* getProcess(int pid);
	void removeProcess(int pid);
	void sendMessage(int pid, Array<byte> msg);
	void sendTerminalInput(int pid, Array<byte> msg);
	bool getRunning();
	void setRunning(bool r);
	int processCount();
};