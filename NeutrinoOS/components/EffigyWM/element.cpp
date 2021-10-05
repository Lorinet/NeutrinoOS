#include "element.h"
map<string, string> properties;
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
	if (properties.find(prop) != properties.end()) return properties[prop];
	else return "";
}
int Element::IGetProperty(string prop)
{
	if (properties.find(prop) != properties.end()) return atoi(properties[prop].c_str());
	else return -1;
}
void Element::SetProperty(string prop, string val)
{
	properties[prop] = val;
}
void Element::SetProperty(string prop, int val)
{
	properties[prop] = to_string(val);
}
bool Element::Hover(int x, int y)
{
	int pw = IGetProperty("Width");
	int ph = IGetProperty("Height");
	int px = IGetProperty("Position X");
	int py = IGetProperty("Position Y");
	if (x >= px && x <= px + pw && y >= py && y <= py + ph) return true;
	return false;
}
Element Element::Label(int x, int y, string font, string text)
{
	return Element({ {"Type", "Label"}, {"Position X", to_string(x)}, {"Position Y", to_string(y)}, {"Font", font }, {"Text", text} });
}
Element Element::Button(int x, int y, int w, int h, string font, string text)
{
	return Element({ {"Type", "Button"}, {"Position X", to_string(x)}, {"Position Y", to_string(y)}, {"Width", to_string(w)}, {"Height", to_string(h)}, {"Font", font }, {"Text", text}, { "Hoverable", "1" } });
}
Element Element::CheckBox(int x, int y, string font, string text)
{
	return Element({ {"Type", "CheckBox"}, {"Position X", to_string(x)}, {"Position Y", to_string(y)}, {"Font", font }, {"Text", text}, {"Checked", "0"}, { "Hoverable", "1" } });
}