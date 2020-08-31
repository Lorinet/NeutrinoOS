#include "winmgr.h"
#include "vmmgr.h"
Color Theming::AccentColor(0, 200, 255, 230);
Color Theming::InactiveColor(255, 255, 255, 230);
Color Theming::DesktopBackgroundColor(0, 20, 26, 255);
Color Theming::WindowBackgroundColor(20, 20, 20, 240);
Color Theming::TransparenceColor(100, 180, 200, 40);
Color Theming::TextColorDark(10, 10, 10, 255);
Color Theming::TextColorLight(220, 220, 220, 255);
Color Theming::CloseButtonHoverColor(220, 0, 0, 255);
Color Theming::CaptionButtonHoverColor(0, 255, 255, 100);
Color Theming::AccentColorOutline(230, 255, 255, 255);
Color Theming::TaskbarColor(0, 200, 255, 180);
string Theming::NeutrinoGraphic = "light";
string Theming::CaptionColor = "dark";

void Theming::ApplyTheme(string path)
{
	ifstream lnth(path);
	for (int i = 0; i < 11; i++)
	{
		int r, g, b, a;
		lnth >> r >> g >> b >> a;
		Color c(r, g, b, a);
		switch (i)
		{
		case 0: AccentColor = c; break;
		case 1: InactiveColor = c; break;
		case 2: DesktopBackgroundColor = c; break;
		case 3: WindowBackgroundColor = c; break;
		case 4: TransparenceColor = c; break;
		case 5: TextColorDark = c; break;
		case 6: TextColorLight = c; break;
		case 7: CloseButtonHoverColor = c; break;
		case 8: CaptionButtonHoverColor = c; break;
		case 9: AccentColorOutline = c; break;
		case 10: TaskbarColor = c; break;
		}
	}
	lnth >> NeutrinoGraphic;
	lnth >> CaptionColor;
}

bool FeatureSet::WindowsEnabled = true;
bool FeatureSet::TaskbarEnabled = true;
bool FeatureSet::Menu = true;
bool FeatureSet::MobileMode = false;

map<int, Window> WindowManager::windows;
int WindowManager::activeWindow = -1;
bool WindowManager::dragging = false;
bool WindowManager::hoveringOnClose = false;
bool WindowManager::hoveringOnMaximize = false;
bool WindowManager::hoveringOnMinimize = false;
vector<Element> WindowManager::taskbar;
void WindowManager::RenderWindows()
{
	if (windows.find(-1) != windows.end()) windows.erase(-1);
	Graphics::SetColor(Theming::DesktopBackgroundColor);
	Graphics::ClearScreen();
	Graphics::SetColor(Theming::TextColorLight);
	Graphics::DrawString(5, 5, aboutNeutrino + " - Only for evaluation purposes", "Helvetica 8");
	if (dragging && activeWindow != -1) RenderWindow(windows[activeWindow], activeWindow, true);
	for (pair<int, Window> w : windows)
	{
		if (w.first != activeWindow) RenderWindow(w.second, w.first, false);
	}
	if (!dragging && activeWindow != -1) RenderWindow(windows[activeWindow], activeWindow, true);
	Graphics::SetColor(Theming::TaskbarColor);
	Graphics::DrawFilledRectangle(0, Graphics::resY - 30, Graphics::resX, 30, Theming::TaskbarColor);
	Graphics::DrawImage(5, Graphics::resY - 28, "icon_" + Theming::NeutrinoGraphic);
	int ti = 37;
	for (Element& e : taskbar)
	{
		int tw = e.IGetProperty("Width");
		e.properties["Position X"] = to_string(ti);
		e.properties["Position Y"] = to_string(Graphics::resY - 30);
		if (e.IGetProperty("Window ID") == activeWindow)
		{
			Graphics::SetColor(Theming::CaptionButtonHoverColor);
			Graphics::DrawFilledRectangle(e.IGetProperty("Position X"), e.IGetProperty("Position Y"), e.IGetProperty("Width"), e.IGetProperty("Height"), Theming::CaptionButtonHoverColor);
			if (e.GetProperty("Hovering") == "1")
			{
				Graphics::SetColor(Theming::TransparenceColor);
				Graphics::DrawFilledRectangle(e.IGetProperty("Position X"), e.IGetProperty("Position Y"), e.IGetProperty("Width"), e.IGetProperty("Height"), Theming::TaskbarColor);
			}
		}
		else
		{
			if (e.GetProperty("Hovering") == "1")
			{
				Graphics::SetColor(Theming::TransparenceColor);
				Graphics::DrawFilledRectangle(e.IGetProperty("Position X"), e.IGetProperty("Position Y"), e.IGetProperty("Width"), e.IGetProperty("Height"), Theming::TransparenceColor);
			}
		}
		Graphics::SetColor(Theming::AccentColorOutline);
		Graphics::DrawFilledRectangle(ti, Graphics::resY - 2, tw, 1, Theming::AccentColorOutline);
		Graphics::SetColor(Theming::TextColorDark);
		Graphics::DrawString(ti + 10, Graphics::resY - 20, e.GetProperty("Text"), "Helvetica 8");
		ti += tw + 8;
	}
	Graphics::UpdateScreen();
}
void WindowManager::RenderWindow(Window& w, int wid, bool active)
{
	if (w.hidden == false)
	{
		Color accent = Theming::AccentColor;
		if (!active && !dragging) accent = Theming::InactiveColor;
		Graphics::SetColor(accent);
		if (dragging)
		{
			if (w.titleBar == 1)
			{
				if (Graphics::glareEnabled) Graphics::DrawFilledRectangle(w.posX, w.posY, w.width, w.height + 30, Theming::TransparenceColor);
				else Graphics::DrawRectangle(w.posX, w.posY, w.width, w.height + 30);
			}
			else
			{
				Graphics::DrawFilledRectangle(w.posX, w.posY + 30, w.width, w.height, Theming::TransparenceColor);
			}
			
		}
		else
		{
			Graphics::DrawFilledRectangle(w.posX, w.posY + 30, w.width, w.height, Theming::WindowBackgroundColor);
			if (w.titleBar == 1)
			{
				Graphics::DrawFilledRectangle(w.posX, w.posY, w.width, 30, accent);
				Graphics::SetColor(Theming::TextColorDark);
				Graphics::DrawString(w.posX + 10, w.posY + 10, w.title, "Helvetica 8");
				Graphics::DrawImage(w.posX + w.width - 35, w.posY + 6, "close");
				if (w.maximized) Graphics::DrawImage(w.posX + w.width - 75, w.posY + 6, "restore");
				else Graphics::DrawImage(w.posX + w.width - 75, w.posY + 6, "maximize");
				Graphics::DrawImage(w.posX + w.width - 115, w.posY + 5, "minimize");
			}
			for (Element& e : w.elements)
			{
				string type = e.GetProperty("Type");
				if (type == "Label")
				{
					Graphics::SetColor(Theming::TextColorLight);
					Graphics::DrawString(e.IGetProperty("Position X") + w.posX, e.IGetProperty("Position Y") + w.posY + w.drawableBase, e.GetProperty("Text"), e.GetProperty("Font"));
				}
				else if (type == "Button")
				{
					if (Graphics::fonts.find(e.GetProperty("Font")) == Graphics::fonts.end()) continue;
					int wi, he;
					TTF_SizeText(Graphics::fonts[e.GetProperty("Font")].font, e.GetProperty("Text").c_str(), &wi, &he);
					Element* ew = &(windows[wid].elements[windows[wid].GetElementIndexByID(e.IGetProperty("ID"))]);
					ew->properties["Width"] = to_string(wi + 10);
					ew->properties["Height"] = to_string(he + 10);
					if (e.GetProperty("Hovering") == "1")
						Graphics::DrawFilledRectangle(e.IGetProperty("Position X") + w.posX, e.IGetProperty("Position Y") + w.posY + w.drawableBase, wi + 10, he + 10, Theming::TransparenceColor);
					Graphics::DrawRectangle(e.IGetProperty("Position X") + w.posX, e.IGetProperty("Position Y") + w.posY + w.drawableBase, wi + 10, he + 10);
					Graphics::DrawString(e.IGetProperty("Position X") + w.posX + 5, e.IGetProperty("Position Y") + w.posY + w.drawableBase + 5, e.GetProperty("Text"), e.GetProperty("Font"));
				}
				else if (type == "CheckBox")
				{
					if (Graphics::fonts.find(e.GetProperty("Font")) == Graphics::fonts.end()) continue;
					int wi, he;
					TTF_SizeText(Graphics::fonts[e.GetProperty("Font")].font, e.GetProperty("Text").c_str(), &wi, &he);
					Element* ew = &(windows[wid].elements[windows[wid].GetElementIndexByID(e.IGetProperty("ID"))]);
					ew->properties["Width"] = to_string(wi + 10);
					ew->properties["Height"] = to_string(he + 10);
					if (e.GetProperty("Hovering") == "1")
						Graphics::DrawFilledRectangle(e.IGetProperty("Position X") + w.posX, e.IGetProperty("Position Y") + w.posY + w.drawableBase, 12, 12, Theming::TransparenceColor);
					if(e.IGetProperty("Checked") == 1) Graphics::DrawFilledRectangle(e.IGetProperty("Position X") + w.posX + 3, e.IGetProperty("Position Y") + w.posY + w.drawableBase + 3, 6, 6, Theming::TextColorLight);
					Graphics::DrawRectangle(e.IGetProperty("Position X") + w.posX, e.IGetProperty("Position Y") + w.posY + w.drawableBase, 12, 12);
					Graphics::DrawString(e.IGetProperty("Position X") + w.posX + 18, e.IGetProperty("Position Y") + w.posY + w.drawableBase, e.GetProperty("Text"), e.GetProperty("Font"));
				}
			}
		}
	}
}
int WindowManager::GetWindowHandle(int x, int y)
{
	if(windows[activeWindow].posX <= x && x <= windows[activeWindow].posX + windows[activeWindow].width && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + windows[activeWindow].height)
		return activeWindow;
	for (pair<int, Window> w : windows)
	{
		if (w.second.posX <= x && x <= w.second.posX + w.second.width && w.second.posY <= y && y <= w.second.posY + w.second.height)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetWindowTitleBarHandle(int x, int y)
{
	if (windows[activeWindow].posX <= x && x <= windows[activeWindow].posX + windows[activeWindow].width - 125 && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + 30)
		return activeWindow;
	else if (windows[activeWindow].posX <= x && x <= windows[activeWindow].posX + windows[activeWindow].width && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + windows[activeWindow].height)
		return -1;
	for (pair<int, Window> w : windows)
	{
		if (w.second.titleBar == 1 && w.second.posX <= x && x <= w.second.posX + w.second.width && w.second.posY <= y && y <= w.second.posY + 30)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetWindowCloseButtonHandle(int x, int y)
{
	if (windows[activeWindow].posX + windows[activeWindow].width - 45 <= x && x <= windows[activeWindow].posX + windows[activeWindow].width && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + 30)
		return activeWindow;
	else if (windows[activeWindow].posX <= x && x <= windows[activeWindow].posX + windows[activeWindow].width && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + windows[activeWindow].height)
		return -1;
	for (pair<int, Window> w : windows)
	{
		if (w.second.titleBar == 1 && w.second.posX + w.second.width - 45 <= x && x <= w.second.posX + w.second.width && w.second.posY <= y && y <= w.second.posY + 30)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetWindowMaximizeButtonHandle(int x, int y)
{
	if (windows[activeWindow].posX + windows[activeWindow].width - 85 <= x && x < windows[activeWindow].posX + windows[activeWindow].width - 45 && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + 30)
		return activeWindow;
	else if (windows[activeWindow].posX <= x && x <= windows[activeWindow].posX + windows[activeWindow].width && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + windows[activeWindow].height)
		return -1;
	for (pair<int, Window> w : windows)
	{
		if (w.second.titleBar == 1 && w.second.posX + w.second.width - 85 <= x && x < w.second.posX + w.second.width - 45 && w.second.posY <= y && y <= w.second.posY + 30)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetWindowMinimizeButtonHandle(int x, int y)
{
	if (windows[activeWindow].posX + windows[activeWindow].width - 125 <= x && x < windows[activeWindow].posX + windows[activeWindow].width - 85 && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + 30)
		return activeWindow;
	else if (windows[activeWindow].posX <= x && x <= windows[activeWindow].posX + windows[activeWindow].width && windows[activeWindow].posY <= y && y <= windows[activeWindow].posY + windows[activeWindow].height)
		return -1;
	for (pair<int, Window> w : windows)
	{
		if (w.second.titleBar == 1 && w.second.posX + w.second.width - 125 <= x && x < w.second.posX + w.second.width - 85 && w.second.posY <= y && y <= w.second.posY + 30)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetTaskbarIndex(int x, int y)
{
	return 1;
}
int WindowManager::CreateWindowE(Window w)
{
	int index = 0;
	while (windows.find(index) != windows.end()) index++;
	windows.insert({ index, w });
	if (w.titleBar == 1)
	{
		Element e = Element();
		e.properties.insert({ "Text", w.title });
		e.properties.insert({ "Width", "150" });
		e.properties.insert({ "Height", "30" });
		e.properties.insert({ "Window ID", to_string(index) });
		e.properties.insert({ "Hoverable", "1" });
		e.properties.insert({ "Hovering", "0" });
		taskbar.push_back(e);
	}
	return index;
}
void WindowManager::CloseWindow(int handle)
{
	if (windows.find(handle) != windows.end())
	{
		int pid = windows[handle].parentProcess;
		bool t = windows[handle].titleBar == 1;
		windows.erase(handle);
		activeWindow = -1;
		for (pair<int, Window> w : windows)
		{
			if (w.second.titleBar == 1)
			{
				activeWindow = w.first;
				break;
			}
		}
		for (int i = 0; i < taskbar.size(); i++)
		{
			if (taskbar[i].IGetProperty("Window ID") == handle && t)
				taskbar.erase(taskbar.begin() + i);
		}
		bool kill = true;
		for (pair<int, Window> w : windows)
		{
			if (w.second.parentProcess == pid) kill = false;
		}
		if (kill)
		{
			vmmgr::terminateProcess(pid);
		}
		RenderWindows();
	}
}
void WindowManager::MoveWindow(int handle, int x, int y)
{
	if (windows.find(handle) != windows.end())
	{
		if (windows[handle].maximized) RestoreDown(handle);
		windows[handle].posX = x;
		windows[handle].posY = y;
	}
	RenderWindows();
}
void WindowManager::ResizeWindow(int handle, int w, int h)
{
	if (windows.find(handle) != windows.end())
	{
		windows[handle].width = w;
		windows[handle].height = h;
	}
	RenderWindows();
}
void WindowManager::Maximize(int handle)
{
	if (windows.find(handle) != windows.end())
	{
		Window* w = &windows[handle];
		if (!w->maximized)
		{
			w->prevPosX = w->posX;
			w->prevPosY = w->posY;
			w->prevW = w->width;
			w->prevH = w->height;
			w->posX = 0;
			w->posY = 0;
			w->width = Graphics::resX - 1;
			w->height = Graphics::resY - 60;
			w->maximized = true;
		}
	}
	RenderWindows();
}
void WindowManager::RestoreDown(int handle)
{
	if (windows.find(handle) != windows.end())
	{
		Window* w = &windows[handle];
		if (w->maximized)
		{
			w->posX = w->prevPosX;
			w->posY = w->prevPosY;
			w->width = w->prevW;
			w->height = w->prevH;
			w->prevPosX = 0;
			w->prevPosY = 0;
			w->prevW = Graphics::resX;
			w->prevH = Graphics::resY - 60;
			w->maximized = false;
		}
	}
	RenderWindows();
}
void WindowManager::HideWindow(int handle)
{
	if (windows.find(handle) != windows.end())
	{
		windows[handle].hidden = true;
		activeWindow = -1;
		for (pair<int, Window> w : windows)
		{
			if (w.second.titleBar == 1 && !w.second.hidden)
			{
				activeWindow = w.first;
				break;
			}
		}
	}
	RenderWindows();
}
void WindowManager::ShowWindow(int handle)
{
	if (windows.find(handle) != windows.end()) windows[handle].hidden = false;
	RenderWindows();
}
void WindowManager::FireEvent(EffigyEvent evt)
{
	if (evt == EffigyEvent::Shutdown)
	{
		SDL_Quit();
		TTF_Quit();
		IMG_Quit();
		exit(0);
	}
}
void WindowManager::Update()
{
	SDL_Event evt;
	int er = SDL_PollEvent(&evt);
	if (er)
	{
		vmmgr::doze(false);
		if (evt.type == SDL_QUIT) WindowManager::FireEvent(EffigyEvent::Shutdown);
		else if (evt.type == SDL_MOUSEBUTTONDOWN)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			WindowManager::FireEvent(EffigyEvent::MouseDown, x, y);
		}
		else if (evt.type == SDL_MOUSEBUTTONUP)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			WindowManager::FireEvent(EffigyEvent::MouseRelease, x, y);
		}
		else if (evt.type == SDL_MOUSEMOTION)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			WindowManager::FireEvent(EffigyEvent::MouseMove, x, y);
		}
		if (evt.type != SDL_MOUSEMOTION)
			WindowManager::RenderWindows();
	}
}
void WindowManager::ErrorScreen(string error)
{
#if defined(__UNIX)
	Graphics::LoadImageE("graphics/face.png", "face");
	Graphics::LoadFont("graphics/fonts/arialb.ttf", "Helvetica 25 Bold", 33);
	Graphics::LoadFont("graphics/fonts/arial.ttf", "Helvetica 14", 19);
	Graphics::LoadFont("graphics/fonts/arialb.ttf", "Helvetica 11 Bold", 15);
	Graphics::LoadFont("graphics/fonts/console.fon", "Console 10", 13);
#elif defined(__WIN32)
	Graphics::LoadImageE("graphics\\face.png", "face");
	Graphics::LoadFont("graphics\\fonts\\arialb.ttf", "Helvetica 25 Bold", 33);
	Graphics::LoadFont("graphics\\fonts\\arial.ttf", "Helvetica 14", 19);
	Graphics::LoadFont("graphics\\fonts\\arialb.ttf", "Helvetica 11 Bold", 15);
	Graphics::LoadFont("graphics\\fonts\\console.fon", "Console 10", 13);
#endif
	Graphics::fontSmoothing = true;
	Graphics::SetColor(0, 100, 255, 255);
	Graphics::ClearScreen();
	Graphics::SetColor(255, 255, 255, 255);
	Graphics::DrawImage(10, 10, "face");
	Graphics::DrawString(130, 45, "Neutrino", "Helvetica 25 Bold");
	Graphics::DrawString(10, 110, "A critical error has been encountered and Neutrino has been shut down", "Helvetica 14");
	Graphics::DrawString(10, 130, "to prevent damage to the system. Please restart your device manually.", "Helvetica 14");
	Graphics::DrawString(10, 200, "Contact your system administrator, device manufacturer or visit the site below", "Helvetica 11 Bold");
	Graphics::DrawString(10, 220, "for more information and possible fixes:", "Helvetica 11 Bold");
	if (Graphics::resY >= 260)
		Graphics::DrawString(10, 245, "http://neutrino.rf.gd/troubleshooting", "Helvetica 14");
	else
		Graphics::DrawString(10, 160, "http://neutrino.rf.gd/troubleshooting", "Helvetica 14");
	if(Graphics::resY >= 480)
		Graphics::DrawString(10, 450, error, "Console 10");
	else
		Graphics::DrawString(10, 5, error, "Console 10");
	Graphics::UpdateScreen();
}
void WindowManager::Initialize()
{
	#if defined(__DESKTOP) && defined(__UNIX)
	setenv("SDL_VIDEODRIVER", "fbcon", 1);
	setenv("SDL_FBDEV", "/dev/fb0", 1);
	#endif
	Graphics::InitGraphicSystem();
	#if defined(__WIN32)
	Graphics::LoadImageE("graphics\\close.png", "close");
	Graphics::LoadImageE("graphics\\closelight.png", "closelight");
	Graphics::LoadImageE("graphics\\maximize.png", "maximize");
	Graphics::LoadImageE("graphics\\restore.png", "restore");
	Graphics::LoadImageE("graphics\\minimize.png", "minimize");
	Graphics::LoadImageE("graphics\\NeutrinoIconLight.png", "icon_light");
	Graphics::LoadImageE("graphics\\NeutrinoIconDark.png", "icon_dark");
	Graphics::LoadFont("graphics\\fonts\\micro.ttf", "Micro 4", 8);
	Graphics::LoadFont("graphics\\fonts\\micro.ttf", "Micro 5", 8);
	Graphics::LoadFont("graphics\\fonts\\console.fon", "Console 7", 9);
	Graphics::LoadFont("graphics\\fonts\\console.fon", "Console 10", 13);
	Graphics::LoadFont("graphics\\fonts\\openico.ttf", "Open Iconic 8", 11);
	Graphics::LoadFont("graphics\\fonts\\openico.ttf", "Open Iconic 16", 22);
	Graphics::LoadFont("graphics\\fonts\\openico.ttf", "Open Iconic 32", 44);
	Graphics::LoadFont("graphics\\fonts\\openico.ttf", "Open Iconic 48", 66);
	Graphics::LoadFont("graphics\\fonts\\openico.ttf", "Open Iconic 64", 88);
	Graphics::LoadFont("graphics\\fonts\\courier.fon", "Courier New 11", 15);
	Graphics::LoadFont("graphics\\fonts\\courier.fon", "Courier New 19", 25);
	Graphics::LoadFont("graphics\\fonts\\7segment.ttf", "Segments 42 Numbers", 56);
	Graphics::LoadFont("graphics\\fonts\\battery.fon", "Battery 19", 25);
	Graphics::LoadFont("graphics\\fonts\\arial.ttf", "Helvetica 8", 11);
	Graphics::LoadFont("graphics\\fonts\\arialb.ttf", "Helvetica 8 Bold", 11);
	Graphics::LoadFont("graphics\\fonts\\arial.ttf", "Helvetica 11", 15);
	Graphics::LoadFont("graphics\\fonts\\arialb.ttf", "Helvetica 11 Bold", 15);
	Graphics::LoadFont("graphics\\fonts\\arial.ttf", "Helvetica 12", 16);
	Graphics::LoadFont("graphics\\fonts\\arial.ttf", "Helvetica 12 Thin", 16);
	Graphics::LoadFont("graphics\\fonts\\arial.ttf", "Helvetica 14", 19);
	Graphics::LoadFont("graphics\\fonts\\arial.ttf", "Helvetica 19", 25);
	Graphics::LoadFont("graphics\\fonts\\arial.ttf", "Helvetica 25", 33);
	Graphics::LoadFont("graphics\\fonts\\arialb.ttf", "Helvetica 25 Bold", 33);
	Graphics::LoadFont("graphics\\fonts\\times.ttf", "Times New Roman 7", 9);
	Graphics::LoadFont("graphics\\fonts\\times.ttf", "Times New Roman 7 Bold", 9);
	Graphics::LoadFont("graphics\\fonts\\times.ttf", "Times New Roman 10", 13);
	Graphics::LoadFont("graphics\\fonts\\timesb.ttf", "Times New Roman 10 Bold", 13);
	Graphics::LoadFont("graphics\\fonts\\times.ttf", "Times New Roman 13", 17);
	Graphics::LoadFont("graphics\\fonts\\timesb.ttf", "Times New Roman 13 Bold", 17);
	Graphics::LoadFont("graphics\\fonts\\times.ttf", "Times New Roman 23", 31);
	Graphics::LoadFont("graphics\\fonts\\logisoso.ttf", "Logisoso 16", 22);
	Graphics::LoadFont("graphics\\fonts\\logisoso.ttf", "Logisoso 20", 27);
	Graphics::LoadFont("graphics\\fonts\\logisoso.ttf", "Logisoso 22", 29);
	Graphics::LoadFont("graphics\\fonts\\logisoso.ttf", "Logisoso 26", 36);
	Graphics::LoadFont("graphics\\fonts\\logisoso.ttf", "Logisoso 28", 37);
	Graphics::LoadFont("graphics\\fonts\\logisoso.ttf", "Logisoso 32", 43);
	Graphics::LoadFont("graphics\\fonts\\logisoso.ttf", "Logisoso 38", 22);
	#elif defined(__UNIX)
	Graphics::LoadImageE("graphics/close.png", "close");
	Graphics::LoadImageE("graphics/closelight.png", "closelight");
	Graphics::LoadImageE("graphics/maximize.png", "maximize");
	Graphics::LoadImageE("graphics/restore.png", "restore");
	Graphics::LoadImageE("graphics/minimize.png", "minimize");
	Graphics::LoadImageE("graphics/NeutrinoIconLight.png", "icon_light");
	Graphics::LoadImageE("graphics/NeutrinoIconDark.png", "icon_dark");
	Graphics::LoadFont("graphics/fonts/micro.ttf", "Micro 4", 8);
	Graphics::LoadFont("graphics/fonts/micro.ttf", "Micro 5", 8);
	Graphics::LoadFont("graphics/fonts/console.fon", "Console 7", 9);
	Graphics::LoadFont("graphics/fonts/console.fon", "Console 10", 13);
	Graphics::LoadFont("graphics/fonts/openico.ttf", "Open Iconic 8", 11);
	Graphics::LoadFont("graphics/fonts/openico.ttf", "Open Iconic 16", 22);
	Graphics::LoadFont("graphics/fonts/openico.ttf", "Open Iconic 32", 44);
	Graphics::LoadFont("graphics/fonts/openico.ttf", "Open Iconic 48", 66);
	Graphics::LoadFont("graphics/fonts/openico.ttf", "Open Iconic 64", 88);
	Graphics::LoadFont("graphics/fonts/courier.fon", "Courier New 11", 15);
	Graphics::LoadFont("graphics/fonts/courier.fon", "Courier New 19", 25);
	Graphics::LoadFont("graphics/fonts/7segment.ttf", "Segments 42 Numbers", 56);
	Graphics::LoadFont("graphics/fonts/battery.fon", "Battery 19", 25);
	Graphics::LoadFont("graphics/fonts/arial.ttf", "Helvetica 8", 11);
	Graphics::LoadFont("graphics/fonts/arialb.ttf", "Helvetica 8 Bold", 11);
	Graphics::LoadFont("graphics/fonts/arial.ttf", "Helvetica 11", 15);
	Graphics::LoadFont("graphics/fonts/arialb.ttf", "Helvetica 11 Bold", 15);
	Graphics::LoadFont("graphics/fonts/arial.ttf", "Helvetica 12", 16);
	Graphics::LoadFont("graphics/fonts/arial.ttf", "Helvetica 12 Thin", 16);
	Graphics::LoadFont("graphics/fonts/arial.ttf", "Helvetica 14", 19);
	Graphics::LoadFont("graphics/fonts/arial.ttf", "Helvetica 19", 25);
	Graphics::LoadFont("graphics/fonts/arial.ttf", "Helvetica 25", 33);
	Graphics::LoadFont("graphics/fonts/arialb.ttf", "Helvetica 25 Bold", 33);
	Graphics::LoadFont("graphics/fonts/times.ttf", "Times New Roman 7", 9);
	Graphics::LoadFont("graphics/fonts/times.ttf", "Times New Roman 7 Bold", 9);
	Graphics::LoadFont("graphics/fonts/times.ttf", "Times New Roman 10", 13);
	Graphics::LoadFont("graphics/fonts/timesb.ttf", "Times New Roman 10 Bold", 13);
	Graphics::LoadFont("graphics/fonts/times.ttf", "Times New Roman 13", 17);
	Graphics::LoadFont("graphics/fonts/timesb.ttf", "Times New Roman 13 Bold", 17);
	Graphics::LoadFont("graphics/fonts/times.ttf", "Times New Roman 23", 31);
	Graphics::LoadFont("graphics/fonts/logisoso.ttf", "Logisoso 16", 22);
	Graphics::LoadFont("graphics/fonts/logisoso.ttf", "Logisoso 20", 27);
	Graphics::LoadFont("graphics/fonts/logisoso.ttf", "Logisoso 22", 29);
	Graphics::LoadFont("graphics/fonts/logisoso.ttf", "Logisoso 26", 36);
	Graphics::LoadFont("graphics/fonts/logisoso.ttf", "Logisoso 28", 37);
	Graphics::LoadFont("graphics/fonts/logisoso.ttf", "Logisoso 32", 43);
	Graphics::LoadFont("graphics/fonts/logisoso.ttf", "Logisoso 38", 22);
	#endif
	if(config::keyExists("effigy\\CurrentTheme")) Theming::ApplyTheme(lvmgr::formatPath(config::getValue("effigy\\CurrentTheme")));
	if (config::keyExists("effigy\\GlareEnabled")) Graphics::glareEnabled = (config::getValueInt("effigy\\GlareEnabled") == 1 ? true : false);
	if (config::keyExists("effigy\\TextSmoothing")) Graphics::fontSmoothing = (config::getValueInt("effigy\\TextSmoothing") == 1 ? true : false);

	RenderWindows();
}
void WindowManager::FireEvent(EffigyEvent evt, int p)
{

}
void WindowManager::FireEvent(EffigyEvent evt, int x, int y)
{
	if (evt == EffigyEvent::MouseDown)
	{
		activeWindow = GetWindowHandle(x, y);
		if (activeWindow != -1)
		{
			if (Graphics::glareEnabled)
			{
				if (windows[activeWindow].titleBar == 1)
				{
					if (GetWindowTitleBarHandle(x, y) == activeWindow)
						dragging = true;
					if (hoveringOnClose) CloseWindow(activeWindow);
					if (hoveringOnMaximize)
					{
						if (windows[activeWindow].maximized) RestoreDown(activeWindow);
						else Maximize(activeWindow);
					}
					if (hoveringOnMinimize) HideWindow(activeWindow);
				}
			}
			else
			{
				if (windows[activeWindow].titleBar == 1)
				{
					if (GetWindowTitleBarHandle(x, y) == activeWindow)
						dragging = true;
					int cbh = GetWindowCloseButtonHandle(x, y);
					if (cbh != -1)
					{
						CloseWindow(cbh);
					}
					int mxbh = GetWindowMaximizeButtonHandle(x, y);
					if (mxbh != -1)
					{
						if (windows[mxbh].maximized) RestoreDown(mxbh);
						else Maximize(mxbh);
					}
					int minbh = GetWindowMinimizeButtonHandle(x, y);
					if (minbh != -1)
					{
						HideWindow(minbh);
					}
				}
			}
		}
		for (pair<int, Window> w : windows)
		{
			for (Element& e : w.second.elements)
			{
				if (e.GetProperty("Hoverable") == "1" || e.GetProperty("Type") == "Button")
				{
					Element* ew = &(windows[w.first].elements[windows[w.first].GetElementIndexByID(e.IGetProperty("ID"))]);
					if (e.Hover(x - w.second.posX, y - w.second.posY - w.second.drawableBase))
					{
						if (find(w.second.properties.begin(), w.second.properties.end(), "WakeOnInteraction") != w.second.properties.end())
						{
							vmmgr::processes[w.second.parentProcess]->suspended = false;
						}
						if (ew->eventHandler != -1)
						{
							vmmgr::processes[w.second.parentProcess]->branch(ew->eventHandler);
						}
					}
				}
				else if (e.GetProperty("Hoverable") == "1" || e.GetProperty("Type") == "CheckBox")
				{
					Element* ew = &(windows[w.first].elements[windows[w.first].GetElementIndexByID(e.IGetProperty("ID"))]);
					if (e.Hover(x - w.second.posX, y - w.second.posY - w.second.drawableBase))
					{
						if (ew->IGetProperty("Checked") == 0) ew->properties["Checked"] = "1";
						else ew->properties["Checked"] = "0";
						if (find(w.second.properties.begin(), w.second.properties.end(), "WakeOnInteraction") != w.second.properties.end())
						{
							vmmgr::processes[w.second.parentProcess]->suspended = false;
						}
						if (ew->eventHandler != -1)
						{
							vmmgr::processes[w.second.parentProcess]->branch(ew->eventHandler);
						}
						RenderWindows();
					}
				}
			}
		}
		for (Element e : taskbar)
		{
			if (Graphics::glareEnabled)
			{
				if (e.GetProperty("Hovering") == "1")
				{
					windows[e.IGetProperty("Window ID")].hidden = false;
					activeWindow = e.IGetProperty("Window ID");
				}
			}
			else
			{
				for (Element& e : taskbar)
				{
					if (e.Hover(x, y))
					{
						windows[e.IGetProperty("Window ID")].hidden = false;
						activeWindow = e.IGetProperty("Window ID");
					}
				}
			}
		}
	}
	else if (evt == EffigyEvent::MouseRelease)
	{
		dragging = false;
		RenderWindows();
	}
	else if (evt == EffigyEvent::MouseMove)
	{
		if (dragging)
		{
			MoveWindow(activeWindow, x, y);
		}
		else if(Graphics::glareEnabled)
		{
			int closeHandle = GetWindowCloseButtonHandle(x, y);
			int maxHandle = GetWindowMaximizeButtonHandle(x, y);
			int minHandle = GetWindowMinimizeButtonHandle(x, y);
			if (closeHandle == -1 && hoveringOnClose)
			{
				hoveringOnClose = false;
				RenderWindows();
			}
			if (maxHandle == -1 && hoveringOnMaximize)
			{
				hoveringOnMaximize = false;
				RenderWindows();
			}
			if (minHandle == -1 && hoveringOnMinimize)
			{
				hoveringOnMinimize = false;
				RenderWindows();
			}
			if (closeHandle != -1)
			{
				if (!hoveringOnClose)
				{
					Window w = windows[closeHandle];
					Graphics::SetColor(Theming::CloseButtonHoverColor);
					Graphics::DrawFilledRectangle(w.posX + w.width - 45, w.posY + 1, 44, 29, Theming::CloseButtonHoverColor);
					Graphics::DrawImage(w.posX + w.width - 35, w.posY + 6, "closelight");
					Graphics::UpdateScreen();
					hoveringOnClose = true;
				}
			}
			if (maxHandle != -1)
			{
				if (!hoveringOnMaximize)
				{
					Window w = windows[maxHandle];
					Graphics::SetColor(Theming::CaptionButtonHoverColor);
					Graphics::DrawFilledRectangle(w.posX + w.width - 85, w.posY + 1, 39, 29, Theming::CaptionButtonHoverColor);
					if (w.maximized) Graphics::DrawImage(w.posX + w.width - 75, w.posY + 6, "restore");
					else Graphics::DrawImage(w.posX + w.width - 75, w.posY + 6, "maximize");
					Graphics::UpdateScreen();
					hoveringOnMaximize = true;
				}
			}
			if (minHandle != -1)
			{
				if (!hoveringOnMinimize)
				{
					Window w = windows[minHandle];
					Graphics::SetColor(Theming::CaptionButtonHoverColor);
					Graphics::DrawFilledRectangle(w.posX + w.width - 125, w.posY + 1, 39, 29, Theming::CaptionButtonHoverColor);
					Graphics::DrawImage(w.posX + w.width - 115, w.posY + 5, "minimize");
					Graphics::UpdateScreen();
					hoveringOnMinimize = true;
				}
			}
			if (!hoveringOnClose && !hoveringOnMaximize && !hoveringOnMinimize)
			{
				for (Element& e : taskbar)
				{
					if (e.Hover(x, y))
					{
						e.properties["Hovering"] = "1";
					}
					else e.properties["Hovering"] = "0";
				}
				for (pair<int, Window> w : windows)
				{
					for (Element& e : w.second.elements)
					{
						if (e.GetProperty("Hoverable") == "1" || e.GetProperty("Type") == "Button" || e.GetProperty("Type") == "CheckBox")
						{
							Element* ew = &(windows[w.first].elements[windows[w.first].GetElementIndexByID(e.IGetProperty("ID"))]);
							if (e.Hover(x - w.second.posX, y - w.second.posY - w.second.drawableBase))
								ew->properties["Hovering"] = "1";
							else
								ew->properties["Hovering"] = "0";
						}
					}
				}
				RenderWindows();
			}
		}
	}
}