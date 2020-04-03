#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include "util.h"
using namespace std;
class wifi
{
public:
	static string currentNetwork;
	static bool connected;
	static void connect(string name, string password);
	static void eventHandler(void* arg, int32_t event_id, void* event_data);
};