#include "uiserialization.h"
View* UISerialization::DeserializeView(string s)
{
	// Type:Button......|Type:Label.....
	Array<string> elementser;
	string cur = "";
	cur += s[0];
	for (int i = 1; i < s.size(); i++)
	{
		if (s[i] == '|' && s[i - 1] != '\\')
		{
			elementser.push(cur);
			cur = "";
		}
		else cur += s[i];
	}
	Array<Element> elements;
	for (int i = 0; i < elementser.size; i++)
	{
		elements.push(DeserializeElement(elementser[i]));
	}
	return new View(elements);
}
Element UISerialization::DeserializeElement(string s)
{
	// Type:Button;Text:Buy: eggs\; milk;Width:45;...
	Array<pair<string, string>> tokens;
	string pname, pvalue = "";
	pname += s[0];
	bool cv = false;
	for (int i = 1; i < s.size(); i++)
	{
		if (s[i] == ':' && s[i - 1] != '\\') cv = true;
		else if (s[i] == ';' && s[i - 1] != '\\')
		{
			cv = false;
			pvalue = util::replaceAll(pvalue, "\\:", ":");
			tokens.push({ pname, pvalue });
			pname = "";
			pvalue = "";
		}
		else if (!cv)
		{
			pname += s[i];
		}
		else if (cv)
		{
			pvalue += s[i];
		}
	}
	Element e;
	for (int i = 0; i < tokens.size; i++)
	{
		e.properties.emplace(tokens[i].first, tokens[i].second);
	}
	return e;
}