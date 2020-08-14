#pragma once
#include "util.h"
#include <string>
#if defined(__ESP32)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
class memorystats
{
public:
    static char* basePtr;
    static void init();
    static string heapFree();
    static string stackFree();
};
#endif