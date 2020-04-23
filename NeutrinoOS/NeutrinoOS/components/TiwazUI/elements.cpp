#include "elements.h"
Element::Element()
{
	properties = map<string, string>();
	eventHandler = -1;
}
Element::Element(map<string, string> props)
{
	properties = props;
	eventHandler = -1;
}
string Element::GetProperty(string prop)
{
	return properties[prop];
}
int Element::GetPropertyInt(string prop)
{
	return atoi(properties[prop].c_str());
}