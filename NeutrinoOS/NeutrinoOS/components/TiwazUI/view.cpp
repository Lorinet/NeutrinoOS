#include "view.h"
View::View()
{
	focusedElement = -1;
}
View::View(vector<Element> e)
{
	focusedElement = -1;
	elements = e;
	for (int i = 0; i < elements.size(); i++)
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
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i].GetPropertyInt("ID") == id) return &elements[i];
	}
	return NULL;
}
int View::GetElementIndexByID(int id)
{
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i].GetPropertyInt("ID") == id) return i;
	}
	return -1;
}
int View::GetLastIndex()
{
	int max = 0;
	for (int i = 0; i < elements.size(); i++)
	{
		if (max < elements[i].GetPropertyInt("ID")) max = elements[i].GetPropertyInt("ID");
	}
	return max;
}
bool View::ContainsSelectables()
{
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements[i].GetProperty("Selectable") == "1") 
			return true;
	}
	return false;
}