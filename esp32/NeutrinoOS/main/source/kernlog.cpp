#include "kernlog.h"
#include "ntime.h"
#include <iostream>
using namespace std;
void ktimestamp()
{
    cout << "[" << (float)ntime::getMillis() / (float)1000 << "] ";
}

void klog(string s)
{
    ktimestamp();
    cout << s << endl;
}

void kerr(string s)
{
    ktimestamp();
    cout << "ERR: " << s;
}

void kwarn(string s)
{
    ktimestamp();
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