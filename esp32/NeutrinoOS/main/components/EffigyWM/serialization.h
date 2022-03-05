#pragma once
#include "window.h"
#include "element.h"
#include <string>
#include <vector>
using namespace std;
class Serialization
{
public:
	static vector<Element> DeserializeView(string s);
	static Element DeserializeElement(string s);
};
string replaceAll(string& source, const string& from, const string& to);