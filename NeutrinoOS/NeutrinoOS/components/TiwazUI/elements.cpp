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
Element Elements::Button(int x, int y, int w, int h, string font, string text)
{
	return Element({ {"Type", "Button"}, {"Position X", to_string(x)}, {"Position Y", to_string(y)}, {"Width", to_string(w)}, {"Height", to_string(h)}, {"Font", font }, {"Text", text} });
}
Element Elements::Label(int x, int y, string font, string text)
{
	return Element({ {"Type", "Label"}, {"Position X", to_string(x)}, {"Position Y", to_string(y)}, {"Font", font }, {"Text", text} });
}
Element Elements::CheckBox(int x, int y, string font, string text)
{
	return Element({ {"Type", "CheckBox"}, {"Position X", to_string(x)}, {"Position Y", to_string(y)}, {"Font", font }, {"Text", text}, {"Checked", "0"} });
}