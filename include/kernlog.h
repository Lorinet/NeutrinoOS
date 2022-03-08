#include <string>
#include <mutex>
using namespace std;

void ktimestamp();
void klog(string s);
void klog(string mod, string s);
void kerr(string s);
void kerr(string mod, string s);
void kwarn(string s);
void kwarn(string mod, string s);