#pragma once
#include <string>
#include "containers.h"
#include "elements.h"
using namespace std;
class View
{
public:
	View();
	View(Array<Element> e);
	View(View& other);
	~View();
	void operator=(View& other);
	int focusedElement;
	int parentProcess;
	Array<Element> elements;
	Array<string> properties;
	Element* GetElementByID(int id);
	int GetElementIndexByID(int id);
	int GetLastIndex();
	bool ContainsSelectables();
};

