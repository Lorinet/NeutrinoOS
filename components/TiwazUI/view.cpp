#include "view.h"
View::View()
{
	focusedElement = -1;
	properties = Array<string>();
}
View::View(Array<Element> e)
{
	focusedElement = -1;
	elements = e;
	properties = Array<string>();
	for (int i = 0; i < elements.size; i++)
	{
		if (elements[i].GetProperty("Selectable") == "1")
		{
			focusedElement = elements[i].GetPropertyInt("ID");
			break;
		}
	}
	for(int i = 0; i < elements.size; i++)
	{
		if(elements[i].GetProperty("Type") == "StickyDraw")
		{
			properties.add("StickyDraw");
		}
		else if(elements[i].GetProperty("Type") == "WakeOnInteraction")
		{
			properties.add("WakeOnInteraction");
		}
		else if(elements[i].GetProperty("Type") == "ListView")
		{
			properties.add("ListView");
			elements[i].SetPropertyInt("SelectedIndex", 0);
			elements[i].SetPropertyInt("FrameStart", 0);
			elements[i].SetPropertyInt("Length", 2);
		}
	}
}
View::View(View& other)
{
	focusedElement = -1;
	elements = Array<Element>();
	for(int i = 0; i < other.elements.size; i++)
	{
		elements.add(other.elements[i]);
	}
	for (int i = 0; i < elements.size; i++)
	{
		if (elements[i].GetProperty("Selectable") == "1")
		{
			focusedElement = elements[i].GetPropertyInt("ID");
			break;
		}
	}
	properties = Array<string>();
	for(int i = 0; i < other.properties.size; i++)
	{
		properties.add(other.properties[i]);
	}
}
View::~View()
{
	elements.clear();
	properties.clear();
	focusedElement = -1;
}
void View::operator=(View& other)
{
	focusedElement = -1;
	elements = Array<Element>();
	for(int i = 0; i < other.elements.size; i++)
	{
		elements.add(other.elements[i]);
	}
	for (int i = 0; i < elements.size; i++)
	{
		if (elements[i].GetProperty("Selectable") == "1")
		{
			focusedElement = elements[i].GetPropertyInt("ID");
			break;
		}
	}
}
Element* View::GetElementByID(int id)
{
	for (int i = 0; i < elements.size; i++)
	{
		if (elements[i].GetPropertyInt("ID") == id) return &elements[i];
	}
	return NULL;
}
int View::GetElementIndexByID(int id)
{
	for (int i = 0; i < elements.size; i++)
	{
		if (elements[i].GetPropertyInt("ID") == id) return i;
	}
	return -1;
}
int View::GetLastIndex()
{
	int max = 0;
	for (int i = 0; i < elements.size; i++)
	{
		if (max < elements[i].GetPropertyInt("ID")) max = elements[i].GetPropertyInt("ID");
	}
	return max;
}
bool View::ContainsSelectables()
{
	for (int i = 0; i < elements.size; i++)
	{
		if (elements[i].GetProperty("Selectable") == "1") 
			return true;
	}
	return false;
}