#pragma once
#include <string>
#include <vector>
#include "elements.h"
using namespace std;
class View
{
public:
	View();
	View(vector<Element> e);
	int focusedElement;
	int parentProcess;
	vector<Element> elements;
	Element* GetElementByID(int id);
	int GetElementIndexByID(int id);
	int GetLastIndex();
	bool ContainsSelectables();
};

