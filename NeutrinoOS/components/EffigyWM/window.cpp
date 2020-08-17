#include "window.h"
Window::Window()
{
	properties = vector<string>();
	posX = Graphics::resX / 3;
	posY = Graphics::resY / 3;
	width = 150;
	height = 120;
	hidden = false;
	maximized = false;
	prevW = width;
	prevH = height;
	prevPosX = posX;
	prevPosY = posY;
	titleBar = 1;
	drawableBase = 30;
}
Window::Window(vector<Element> el)
{
	properties = vector<string>();
	posX = Graphics::resX / 3;
	posY = Graphics::resY / 3;
	width = 150;
	height = 120;
	hidden = false;
	maximized = false;
	prevW = width;
	prevH = height;
	prevPosX = posX;
	prevPosY = posY;
	titleBar = 1;
	drawableBase = 30;
	elements = el;
	for (int i = 0; i < el.size(); i++)
	{
		if (el[i].GetProperty("Type") == "StickyDraw")
		{
			properties.push_back("StickyDraw");
		}
		else if (el[i].GetProperty("Type") == "WakeOnInteraction")
		{
			properties.push_back("WakeOnInteraction");
		}
		else if (el[i].GetProperty("Type") == "WindowInfo")
		{
			title = el[i].GetProperty("Text");
			width = el[i].IGetProperty("Width");
			height = el[i].IGetProperty("Height");
		}
	}
}
Window::Window(int x, int y, int w, int h, string ttl, int titleBarStyle)
{
	posX = x;
	posY = y;
	width = w;
	height = h;
	hidden = false;
	title = ttl;
	maximized = false;
	prevW = width;
	prevH = height;
	prevPosX = posX;
	prevPosY = posY;
	if (titleBarStyle == 0)
	{
		drawableBase = 0;
	}
	else if (titleBarStyle == 1)
	{
		drawableBase = 30;
	}
	titleBar = titleBarStyle;
}
void Window::AddElement(Element e)
{
	elements.push_back(e);
}
void Window::RemoveElement(int id)
{
	int index = GetElementIndexByID(id);
	if (index != -1)
	{
		elements.erase(elements.begin() + index);
	}
}
Element* Window::GetElementByID(int id)
{
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i].IGetProperty("ID") == id) return &elements[i];
	}
	return NULL;
}
int Window::GetElementIndexByID(int id)
{
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i].IGetProperty("ID") == id) return i;
	}
	return -1;
}
Element* Window::GetElementAt(int x, int y)
{
	for (Element& e : elements)
	{
		if (stoi(e.properties["PositionX"]) <= x && x <= stoi(e.properties["PositionX"]) + stoi(e.properties["Width"]) && stoi(e.properties["PositionY"]) <= y &&  y <= stoi(e.properties["PositionY"]) + stoi(e.properties["Height"]))
		{
			return &e;
		}
	}
	return NULL;
}
