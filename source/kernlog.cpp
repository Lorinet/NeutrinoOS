#include "kernlog.h"
#include "ntime.h"
#include "memorystats.h"
#include <iostream>
using namespace std;

mutex coutMutex;

void ktimestamp()
{
    lock_guard<mutex> lock(coutMutex);
    cout << "[" << (float)ntime::getMillis() / (float)1000 << "] ";
}

void klog(string s)
{
    ktimestamp();
    lock_guard<mutex> lock(coutMutex);
    cout << s << endl;
}

void kerr(string s)
{
    ktimestamp();
    lock_guard<mutex> lock(coutMutex);
    cout << "ERR: " << s;
}

void kwarn(string s)
{
    ktimestamp();
    lock_guard<mutex> lock(coutMutex);
    cout << "WARN: " << s;
}

void klog(string mod, string s)
{
    klog(mod + ": " + s);
}

void kerr(string mod, string s)
{
    kerr(mod + ": " + s);
}

void kwarn(string mod, string s)
{
    kwarn(mod + ": " + s);
}