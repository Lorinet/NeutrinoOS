#pragma once
#include <map>
#include "view.h"
#include "graphics.h"
#include "vmmgr.h"
#include "memorystats.h"
using namespace std;
enum TiwazEvent
{
	BackButton,
	SelectButton,
	EnterButton,
	HomeButton
};
class ViewManager
{
public:
	static map<int, View*> views;
	static int activeView;
	static void Initialize();
	static void RenderView(View* v);
	static int CreateView(View* v);
	static void CloseView(int handle);
	static void FireEvent(TiwazEvent e);
};

