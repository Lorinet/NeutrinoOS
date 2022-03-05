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
	if(properties.find(prop) != properties.end()) return properties[prop];
	else return "";
}
int Element::GetPropertyInt(string prop)
{
	if(properties.find(prop) != properties.end()) return atoi(properties[prop].c_str());
	else return 0;
}
void Element::SetProperty(string prop, string value)
{
	if(properties.find(prop) == properties.end()) properties.emplace(prop, value);
	else properties[prop] = value;
}
void Element::SetPropertyInt(string prop, int value)
{
	if(properties.find(prop) == properties.end()) properties.emplace(prop, to_string(value));
	else properties[prop] = to_string(value);
}