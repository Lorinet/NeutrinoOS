#pragma once
#include "view.h"
#include "elements.h"
#include "util.h"
#include <string>
#include "containers.h"
using namespace std;
class UISerialization
{
public:
	static View* DeserializeView(string s);
	static Element DeserializeElement(string s);
};