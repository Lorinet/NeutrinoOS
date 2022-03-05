#pragma once
#include "components.h"

#ifdef COMPONENT_EFFIGY
#include <map>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include "window.h"
#include "iapi.h"
#include "containers.h"
#include "util.h"
#include "bitconverter.h"

class nvm;
class vmmgr;
struct Color;

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

// API compatibility with Tiwaz
class winmgr_api : public iapi
{
public:
	static winmgr_api instance;
	static const int id = 0;
	static void initialize();
	Array<byte> message(Array<byte> indata, nvm* v);
};

enum class EffigyEvent
{
	MouseDown,
	MouseRelease,
	MouseMove,
	Click,
	KeyPressed,
	Shutdown
};

class FeatureSet
{
public:
	static bool WindowsEnabled;
	static bool TaskbarEnabled;
	static bool Menu;
	static bool MobileMode;
};

class Theming
{
public:
	static Color AccentColor;
	static Color InactiveColor;
	static Color DesktopBackgroundColor;
	static Color WindowBackgroundColor;
	static Color TransparenceColor;
	static Color TextColorDark;
	static Color TextColorLight;
	static Color CloseButtonHoverColor;
	static Color CaptionButtonHoverColor;
	static Color AccentColorOutline;
	static Color TaskbarColor;
	static string NeutrinoGraphic;
	static string CaptionColor;
	static void ApplyTheme(string path);
};

class WindowManager
{
public:
	static map<int, Window> windows;
	static vector<Element> taskbar;
	static thread updateThread;
	static int activeWindow;
	static bool dragging;
	static bool hoveringOnClose;
	static bool hoveringOnMaximize;
	static bool hoveringOnMinimize;
	static void Initialize();
	static void SDLThread();
	static void Stop();
	static void ErrorScreen(string error);
	static void InvokeRedraw();
	static void RenderWindows();
	static void RenderWindow(Window& w, int wid, bool active);
	static int GetWindowHandle(int x, int y);
	static int GetWindowTitleBarHandle(int x, int y);
	static int GetWindowCloseButtonHandle(int x, int y);
	static int GetWindowMaximizeButtonHandle(int x, int y);
	static int GetWindowMinimizeButtonHandle(int x, int y);
	static int GetTaskbarIndex(int x, int y);
	static int CreateWindowQ(Window w);
	static int CreateWindowE(Window w);
	static void CloseWindow(int handle);
	static void MoveWindow(int handle, int x, int y);
	static void ResizeWindow(int handle, int w, int h);
	static void HideWindow(int handle);
	static void ShowWindow(int handle);
	static void Maximize(int handle);
	static void RestoreDown(int handle);
	static void QueueEvent(EffigyEvent evt, int x, int y);
	static void FireEvent(EffigyEvent evt);
	static void FireEvent(EffigyEvent evt, int x, int y);
	static void FireEvent(EffigyEvent evt, int p);
};
#endif