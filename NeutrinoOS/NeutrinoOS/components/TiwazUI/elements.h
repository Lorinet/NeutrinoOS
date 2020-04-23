#pragma once
#include <string>
#include <map>
using namespace std;
class Element
{
public:
	int eventHandler;
	Element();
	Element(map<string, string> props);
	map<string, string> properties;
	string GetProperty(string prop);
	int GetPropertyInt(string prop);
};