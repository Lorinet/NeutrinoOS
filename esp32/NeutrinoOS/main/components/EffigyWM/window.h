#pragma once
#include "element.h"
#include <vector>

struct WindowInfo;

class Window
{
public:
	int parentProcess;
	vector<Element> elements;
	Window();
	Window(vector<Element> el);
	Window(int x, int y, int w, int h, string ttl, int titleBarStyle);
	WindowInfo GetWindowInfo();
	void SetWindowInfo(WindowInfo wi);
	void AddElement(Element e);
	void RemoveElement(int id);
	Element* GetElementByID(int id);
	int GetElementIndexByID(int id);
	Element* GetElementAt(int x, int y);
};
