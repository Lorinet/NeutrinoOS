#include "window.h"
#include "graphics.h"
#include "windowinfo.h"
Window::Window()
{
	/*posX = Graphics::resX / 3;
	posY = Graphics::resY / 3;
	width = 150;
	height = 120;*/
}
Window::Window(vector<Element> el)
{
	/*posX = Graphics::resX / 3;
	posY = Graphics::resY / 3;
	width = 150;
	height = 120;*/
	elements = el;
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
WindowInfo Window::GetWindowInfo()
{
	return WindowInfo(this);
}
void Window::SetWindowInfo(WindowInfo wi)
{
	Element* e = GetElementByID(0);
	e->SetProperty("Position X", wi.X);
	e->SetProperty("Position Y", wi.Y);
	e->SetProperty("Prev X", wi.PrevX);
	e->SetProperty("Prev Y", wi.PrevY);
	e->SetProperty("Width", wi.Width);
	e->SetProperty("Height", wi.Height);
	e->SetProperty("Prev Width", wi.PrevWidth);
	e->SetProperty("Prev Height", wi.PrevHeight);
	e->SetProperty("Title", wi.Title);
	e->SetProperty("TitleBar", wi.TitleBar);
	e->SetProperty("MaximizeButton", wi.MaximizeButton);
	e->SetProperty("Hidden", wi.Hidden);
	e->SetProperty("Maximized", wi.Maximized);
	e->SetProperty("StickyDraw", wi.StickyDraw);
	e->SetProperty("WakeOnInteraction", wi.WakeOnInteraction);
	e->SetProperty("Window ID", wi.WindowID);
}