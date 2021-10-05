#pragma once
#include <string>
#include <map>
using namespace std;
class Element
{
public:
	int eventHandler;
	map<string, string> properties;
	Element();
	Element(map<string, string> props);
	string GetProperty(string prop);
	int IGetProperty(string prop);
	void SetProperty(string prop, string val);
	void SetProperty(string prop, int val);
	bool Hover(int x, int y);
	static Element Label(int x, int y, string font, string text);
	static Element Button(int x, int y, int w, int h, string font, string text);
	static Element CheckBox(int x, int y, string font, string text);
};
