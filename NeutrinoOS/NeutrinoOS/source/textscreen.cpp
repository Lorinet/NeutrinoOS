#include "textscreen.h"
void textscreen::print(string ps)
{
	cout << ps;
}
void textscreen::println()
{
	cout << endl;
}
void textscreen::println(string ps)
{
	cout << ps << endl;
}
void textscreen::clear()
{
#ifdef __WIN32
	system("cls");
#endif
}