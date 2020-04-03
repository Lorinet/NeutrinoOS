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
class Elements
{
public:
	static Element Button(int x, int y, int w, int h, string font, string text);
	static Element Label(int x, int y, string font, string text);
	static Element CheckBox(int x, int y, string font, string text);
};