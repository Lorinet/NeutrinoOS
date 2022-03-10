#pragma once
#include <thread>
#include <atomic>
#include <algorithm>
#include <vector>
#include <map>
#include <mutex>
#include "util.h"
#include "scheduler.h"
#if defined(__ESP32)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#elif defined(__WIN32)
#include <Windows.h>
#endif
using namespace std;
class nvm;
class vt;

class vmmgr
{
private:
	static mutex kernelMutex;
	static mutex processIdsMutex;
	static IntMap<int> processIds;
public:
	static int numThreads;
	static atomic<bool> running;
	static bool dozing;
	static int minCpuFreq;
	static int maxCpuFreq;
	static scheduler* schedulers;
	static thread kernelLoopThread;
	static void start();
	static void shutdown();
	static int createProcess(string file, bool start = true);
	static int createProcessEx(string file, vt in, vt out);
	static nvm* getProcess(int pid);
	static void terminateProcess(int pid);
	static void sendMessage(int pid, Array<byte> msg);
	static void sendInput(int pid, Array<byte> input);
	static void enterCriticalSection();
	static void leaveCriticalSection();
	static void doze(bool d);
	static void vmmerror(string error);
	static void vmmerror(string error, int procid);
	static bool inputRequested(int pid);
	static void kernelLoop();
	static void releaseProcessID(int pid);
};