#pragma once
#include <iostream>
#include <string>
#include "util.h"
using namespace std;
class textscreen
{
public:
	static void print(string ps);
	static void println();
	static void println(string ps);
	static void clear();
};