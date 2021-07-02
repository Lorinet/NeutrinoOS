#include "memorystats.h"
#include "kernlog.h"
char *memorystats::basePtr;
void memorystats::init()
{
    char c;
    basePtr = &c;
}
string memorystats::heapFree()
{
    return std::to_string(heap_caps_get_free_size(MALLOC_CAP_8BIT));
}
string memorystats::stackFree()
{
    return std::to_string(uxTaskGetStackHighWaterMark(NULL));
    //char c;
    //return std::to_string(basePtr - &c);
}