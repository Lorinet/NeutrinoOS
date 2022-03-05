#pragma once
#include "components.h"

#ifdef COMPONENT_TIWAZ
#include <map>
#include "view.h"
#include "iapi.h"

using namespace std;

class nvm;

enum uicmd
{
	CreateView = 0,
	DestroyView = 1,
	AddElement = 2,
	ModifyElement = 3,
	DeleteElement = 4,
	GetPropertyValue = 5,
	SetPropertyValue = 6,
	SwitchView = 7,
	UpdateScreen = 8,
	AttachEventHandler = 9,
	DetachEventHandler = 10
};

// API compatibility with Effigy
class winmgr_api : public iapi
{
public:
	static winmgr_api instance;
	static const int id = 0;
	static void initialize();
	Array<byte> message(Array<byte> indata, nvm* v);
};

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

#endif