#pragma once
#include "graphics.h"
#include "element.h"
#include <vector>
class Window
{
public:
	int prevPosX;
	int prevPosY;
	int prevW;
	int prevH;
	int posX;
	int posY;
	int width;
	int height;
	int titleBar;
	int drawableBase;
	int parentProcess;
	bool hidden;
	bool maximized;
	string title;
	vector<Element> elements;
	vector<string> properties;
	Window();
	Window(vector<Element> el);
	Window(int x, int y, int w, int h, string ttl, int titleBarStyle);
	void AddElement(Element e);
	void RemoveElement(int id);
	Element* GetElementByID(int id);
	int GetElementIndexByID(int id);
	Element* GetElementAt(int x, int y);
};

