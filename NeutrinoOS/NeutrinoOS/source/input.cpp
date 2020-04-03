#include "input.h"
#include "nvm.h"
char input::readKey()
{
	return getchar();
}
string input::readLine()
{
	string s;
	getline(cin, s);
	return s;
}
void input::inputLoop(nvm* v)
{
	if (v->awaitin)
	{
		if (v->interm->type == vtype::StandardInput)
		{
			v->interm->registerInputString(readLine());
			v->interm->input.push_back('\n');
		}
		else if (v->interm->type == vtype::Trash)
		{
			v->interm->input.push_back('\n');
		}
	}
}