#include "winmgr.h"

#ifdef COMPONENT_EFFIGY
#include "vmmgr.h"
#include "sysinfo.h"
#include "filesystem.h"
#include "lvmgr.h"
#include "config.h"
#include "graphics.h"
#include "windowinfo.h"
#include "nvm.h"

winmgr_api winmgr_api::instance;

void winmgr_api::initialize()
{
	instance = winmgr_api();
	WindowManager::Initialize();
}

Array<byte> winmgr_api::message(Array<byte> indata, nvm* v)
{
	string txt = "";
	int id = 0;
	int id2 = 0;
	int proc = 0;
	bool bl = false;
	string txt1 = "";
	string txt2 = "";
	switch (indata[0])
	{
	case uicmd::CreateView:
		id = WindowManager::CreateWindowE(Serialization::DeserializeView(bitconverter::tostring(indata, 1)));
		WindowManager::windows[id].parentProcess = v->processid;
		WindowManager::InvokeRedraw();
		return bitconverter::toArray(id);
		break;
	case uicmd::DestroyView:
		WindowManager::CloseWindow(bitconverter::toint32(indata, 1));
		WindowManager::InvokeRedraw();
		break;
	case uicmd::AddElement:
		id = bitconverter::toint32(indata, 1);
		txt = bitconverter::tostring(indata, 5);
		WindowManager::windows[id].elements.push_back(Serialization::DeserializeElement(txt));
		WindowManager::InvokeRedraw();
		break;
	case uicmd::ModifyElement:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		txt = bitconverter::tostring(indata, 9);
		*WindowManager::windows[id].GetElementByID(id2) = Serialization::DeserializeElement(txt);
		WindowManager::InvokeRedraw();
		break;
	case uicmd::DeleteElement:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		WindowManager::windows[id].elements.erase(WindowManager::windows[id].elements.begin() + WindowManager::windows[id].GetElementIndexByID(id2));
		WindowManager::InvokeRedraw();
		break;
	case uicmd::GetPropertyValue:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		txt = bitconverter::tostring(indata, 9);
		return bitconverter::toArray(WindowManager::windows[id].elements[WindowManager::windows[id].GetElementIndexByID(id2)].GetProperty(txt));
		break;
	case uicmd::SetPropertyValue:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		txt = bitconverter::tostring(indata, 9);
		bl = false;
		txt1 = "";
		txt2 = "";
		for (int i = 0; i < txt.size(); i++)
		{
			if (txt[i] == ':' && txt[i - 1] != '\\')
			{
				bl = true;
			}
			else if (!bl) txt1 += txt[i];
			else if (bl) txt2 += txt[i];
		}
		WindowManager::windows[id].elements[WindowManager::windows[id].GetElementIndexByID(id2)].properties[txt1] = util::replaceAll(txt2, "\\:", ":");
		WindowManager::InvokeRedraw();
		break;
	case uicmd::SwitchView:
		id = bitconverter::toint32(indata, 1);
		WindowManager::activeWindow = id;
		WindowManager::InvokeRedraw();
		break;
	case uicmd::UpdateScreen:
		WindowManager::InvokeRedraw();
		break;
	case uicmd::AttachEventHandler:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		proc = bitconverter::toint32(indata, 9);
		WindowManager::windows[id].elements[WindowManager::windows[id].GetElementIndexByID(id2)].eventHandler = proc;
		break;
	case uicmd::DetachEventHandler:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		WindowManager::windows[id].elements[WindowManager::windows[id].GetElementIndexByID(id2)].eventHandler = -1;
		break;
	}
	return Array<byte>();
}

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
thread WindowManager::updateThread;
void WindowManager::Initialize()
{
	updateThread = thread(SDLThread);
}
void WindowManager::SDLThread()
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
	if (config::keyExists("effigy\\CurrentTheme")) Theming::ApplyTheme(lvmgr::formatPath(config::getValue("effigy\\CurrentTheme")));
	if (config::keyExists("effigy\\GlareEnabled")) Graphics::glareEnabled = (config::getValueInt("effigy\\GlareEnabled") == 1 ? true : false);
	if (config::keyExists("effigy\\TextSmoothing")) Graphics::fontSmoothing = (config::getValueInt("effigy\\TextSmoothing") == 1 ? true : false);
	RenderWindows();
	while (true)
	{
		SDL_Event evt;
		int er = SDL_WaitEvent(&evt);
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
			if (evt.type != SDL_MOUSEMOTION) WindowManager::RenderWindows();
		}
	}
}
void WindowManager::InvokeRedraw()
{
	SDL_Event se;
	se.type = SDL_USEREVENT;
	SDL_PushEvent(&se);

}
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
	WindowInfo wi = w.GetWindowInfo();
	if (wi.Hidden == false)
	{
		Color accent = Theming::AccentColor;
		if (!active && !dragging) accent = Theming::InactiveColor;
		Graphics::SetColor(accent);
		if (dragging)
		{
			if (wi.TitleBar)
			{
				if (Graphics::glareEnabled) Graphics::DrawFilledRectangle(wi.X, wi.Y, wi.Width, wi.Height + 30, Theming::TransparenceColor);
				else Graphics::DrawRectangle(wi.X, wi.Y, wi.Width, wi.Height + 30);
			}
			else
			{
				Graphics::DrawFilledRectangle(wi.X, wi.Y + 30, wi.Width, wi.Height, Theming::TransparenceColor);
			}
			
		}
		else
		{
			Graphics::DrawFilledRectangle(wi.X, wi.Y + 30, wi.Width, wi.Height, Theming::WindowBackgroundColor);
			if (wi.TitleBar)
			{
				Graphics::DrawFilledRectangle(wi.X, wi.Y, wi.Width, 30, accent);
				Graphics::SetColor(Theming::TextColorDark);
				Graphics::DrawString(wi.X + 10, wi.Y + 10, wi.Title, "Helvetica 8");
				Graphics::DrawImage(wi.X + wi.Width - 35, wi.Y + 6, "close");
				if (wi.MaximizeButton)
				{
					if (wi.Maximized) Graphics::DrawImage(wi.X + wi.Width - 75, wi.Y + 6, "restore");
					else Graphics::DrawImage(wi.X + wi.Width - 75, wi.Y + 6, "maximize");
					Graphics::DrawImage(wi.X + wi.Width - 115, wi.Y + 5, "minimize");
				}
				else
					Graphics::DrawImage(wi.X + wi.Width - 75, wi.Y + 5, "minimize");
			}
			for (Element& e : w.elements)
			{
				string type = e.GetProperty("Type");
				if (type == "Label")
				{
					Graphics::SetColor(Theming::TextColorLight);
					Graphics::DrawString(e.IGetProperty("Position X") + wi.X, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase, e.GetProperty("Text"), e.GetProperty("Font"));
				}
				else if (type == "Button")
				{
					if (Graphics::fonts.find(e.GetProperty("Font")) == Graphics::fonts.end()) continue;
					int wid, hei;
					TTF_SizeText(Graphics::fonts[e.GetProperty("Font")].font, e.GetProperty("Text").c_str(), &wid, &hei);
					Element* ew = &(windows[wid].elements[windows[wid].GetElementIndexByID(e.IGetProperty("ID"))]);
					ew->properties["Width"] = to_string(wid + 10);
					ew->properties["Height"] = to_string(hei + 10);
					if (e.GetProperty("Hovering") == "1")
						Graphics::DrawFilledRectangle(e.IGetProperty("Position X") + wi.X, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase, wid + 10, hei + 10, Theming::TransparenceColor);
					Graphics::DrawRectangle(e.IGetProperty("Position X") + wi.X, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase, wid + 10, hei + 10);
					Graphics::DrawString(e.IGetProperty("Position X") + wi.X + 5, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase + 5, e.GetProperty("Text"), e.GetProperty("Font"));
				}
				else if (type == "CheckBox")
				{
					if (Graphics::fonts.find(e.GetProperty("Font")) == Graphics::fonts.end()) continue;
					int wid, hei;
					TTF_SizeText(Graphics::fonts[e.GetProperty("Font")].font, e.GetProperty("Text").c_str(), &wid, &hei);
					Element* ew = &(windows[wid].elements[windows[wid].GetElementIndexByID(e.IGetProperty("ID"))]);
					ew->properties["Width"] = to_string(wid + 10);
					ew->properties["Height"] = to_string(hei + 10);
					if (e.GetProperty("Hovering") == "1")
						Graphics::DrawFilledRectangle(e.IGetProperty("Position X") + wi.X, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase, 12, 12, Theming::TransparenceColor);
					if(e.IGetProperty("Checked") == 1) Graphics::DrawFilledRectangle(e.IGetProperty("Position X") + wi.X + 3, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase + 3, 6, 6, Theming::TextColorLight);
					Graphics::DrawRectangle(e.IGetProperty("Position X") + wi.X, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase, 12, 12);
					Graphics::DrawString(e.IGetProperty("Position X") + wi.X + 18, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase, e.GetProperty("Text"), e.GetProperty("Font"));
				}
				else if (type == "TextField")
				{
					int wid = e.IGetProperty("Width");
					int hei = e.IGetProperty("Height");
					if (wid == 0)
					{
						wid = Graphics::resX;
					}
					if (hei == 0)
					{
						hei = Graphics::resX;
					}
					//Graphics::DrawStringWordWrap(e.IGetProperty("Position X"), e.IGetProperty("Position Y"), wid, hei, e.GetProperty("Text"), e.GetProperty("Font"));
					if (e.IGetProperty("Border") == 1)
					{
						Graphics::DrawRectangle(e.IGetProperty("Position X"), e.IGetProperty("Position Y"), wid, hei);
					}
				}
				else if (type == "TextBuffer")
				{
					/*Graphics::SetPos(e->GetPropertyInt("Cursor X"), e->GetPropertyInt("Cursor Y"));
					int curx = 0;
					int cury = 0;
					Graphics::DrawStringWordWrapEx(e->GetProperty("Text"), Graphics::fonts[e->GetProperty("Font")], curx, cury);
					e->SetPropertyInt("Cursor X", curx);
					e->SetPropertyInt("Cursor Y", cury);*/
				}
				else if (type == "ListView")
				{
					Array<string> keys = Array<string>();
					string ck = "";
					string el = e.GetProperty("Items");
					bool truly;
					for (int i = 0; i < el.size(); i++)
					{
						truly = true;
						if (el[i] == ',')
						{
							if (i == 1)
							{
								if (el[0] == '\\')
									truly = false;
							}
							else if (i > 1)
							{
								if (el[i - 1] == '\\' && el[i - 2] != '\\')
									truly = false;
							}
							if (truly)
							{
								keys.add(ck);
								ck = "";
							}
						}
						else
						{
							truly = false;
						}
						if (!truly)
						{
							ck += el[i];
						}
					}
					e.SetProperty("Length", keys.size);
					int ww = e.IGetProperty("Width");
					int wh = e.IGetProperty("Height");
					if (ww <= 0)
					{
						ww = wi.Width;
					}
					if (wh <= 0)
					{
						wh = wi.Height;
					}
					if (e.IGetProperty("FrameStart") == -1) e.SetProperty("FrameStart", 0);
					int yp = 0;
					int eh = TTF_FontHeight(Graphics::fonts[e.GetProperty("Font")].font) + 2;
					int meos = wh / eh;
					int fs = e.IGetProperty("FrameStart");
					if (keys.size > 0)
					{
						if (fs > 0)
						{
							Graphics::SetColor(Theming::TextColorLight);
							Graphics::DrawString(e.IGetProperty("Position X") + wi.X + 2, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase + yp + 2, "...", e.GetProperty("Font"));
							yp += TTF_FontHeight(Graphics::fonts[e.GetProperty("Font")].font) + 2;
							if (keys.size > fs + meos) meos--;
						}
						for (int i = fs; i < fs + meos - 1; i++)
						{
							if (e.IGetProperty("SelectedIndex") == i)
							{
								Graphics::SetColor(Theming::AccentColor);
								Graphics::DrawFilledRectangle(e.IGetProperty("Position X") + wi.X, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase + yp, ww, TTF_FontHeight(Graphics::fonts[e.GetProperty("Font")].font) + 4, Theming::AccentColor);
								Graphics::SetColor(Theming::TextColorDark);
							}
							else Graphics::SetColor(Theming::TextColorLight);
							Graphics::DrawStringBounds(e.IGetProperty("Position X") + wi.X + 2, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase + yp + 2, util::replaceAll(keys[i], "\\\\", "\\"), e.GetProperty("Font"), ww - 5);
							yp += TTF_FontHeight(Graphics::fonts[e.GetProperty("Font")].font) + 2;
							if (i == keys.size - 1) break;
						}
						if (keys.size > fs + meos)
						{
							Graphics::SetColor(Theming::TextColorLight);
							Graphics::DrawString(e.IGetProperty("Position X") + wi.X + 2, e.IGetProperty("Position Y") + wi.Y + wi.DrawableBase + yp + 2, "...", e.GetProperty("Font"));
						}
						keys.clear();
					}
				}
			}
		}
	}
}
int WindowManager::GetWindowHandle(int x, int y)
{
	WindowInfo actWi = windows[activeWindow].GetWindowInfo();
	if(actWi.X <= x && x <= actWi.X + actWi.Width && actWi.Y <= y && y <= actWi.Y + actWi.Height + actWi.DrawableBase)
		return activeWindow;
	for (pair<int, Window> w : windows)
	{
		WindowInfo secWi = w.second.GetWindowInfo();
		if (secWi.X <= x && x <= secWi.X + secWi.Width && secWi.Y <= y && y <= secWi.Y + secWi.Height + secWi.DrawableBase)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetWindowTitleBarHandle(int x, int y)
{
	WindowInfo actWi = windows[activeWindow].GetWindowInfo();
	if (actWi.X <= x && x <= actWi.X + actWi.Width - (actWi.MaximizeButton ? 125 : 85) && actWi.Y <= y && y <= actWi.Y + 30)
		return activeWindow;
	else if (actWi.X <= x && x <= actWi.X + actWi.Width && actWi.Y <= y && y <= actWi.Y + actWi.Height)
		return -1;
	for (pair<int, Window> w : windows)
	{
		WindowInfo secWi = w.second.GetWindowInfo();
		if (secWi.TitleBar && secWi.X <= x && x <= secWi.X + secWi.Width && secWi.Y <= y && y <= secWi.Y + 30)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetWindowCloseButtonHandle(int x, int y)
{
	WindowInfo actWi = windows[activeWindow].GetWindowInfo();
	if (actWi.X + actWi.Width - 45 <= x && x <= actWi.X + actWi.Width && actWi.Y <= y && y <= actWi.Y + 30)
		return activeWindow;
	else if (actWi.X <= x && x <= actWi.X + actWi.Width && actWi.Y <= y && y <= actWi.Y + actWi.Height)
		return -1;
	for (pair<int, Window> w : windows)
	{
		WindowInfo secWi = w.second.GetWindowInfo();
		if (secWi.TitleBar == 1 && secWi.X + secWi.Width - 45 <= x && x <= secWi.X + secWi.Width && secWi.Y <= y && y <= secWi.Y + 30)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetWindowMaximizeButtonHandle(int x, int y)
{
	WindowInfo actWi = windows[activeWindow].GetWindowInfo();
	if (actWi.X + actWi.Width - 85 <= x && x < actWi.X + actWi.Width - 45 && actWi.Y <= y && y <= actWi.Y + 30)
		return activeWindow;
	else if (actWi.X <= x && x <= actWi.X + actWi.Width && actWi.Y <= y && y <= actWi.Y + actWi.Height)
		return -1;
	for (pair<int, Window> w : windows)
	{
		WindowInfo secWi = w.second.GetWindowInfo();
		if (secWi.TitleBar == 1 && secWi.X + secWi.Width - 85 <= x && x < secWi.X + secWi.Width - 45 && secWi.Y <= y && y <= secWi.Y + 30)
		{
			return w.first;
		}
	}
	return -1;
}
int WindowManager::GetWindowMinimizeButtonHandle(int x, int y)
{
	WindowInfo actWi = windows[activeWindow].GetWindowInfo();
	if (actWi.X + actWi.Width - 125 <= x && x < actWi.X + actWi.Width - 85 && actWi.Y <= y && y <= actWi.Y + 30)
		return activeWindow;
	else if (actWi.X <= x && x <= actWi.X + actWi.Width && actWi.Y <= y && y <= actWi.Y + actWi.Height)
		return -1;
	for (pair<int, Window> w : windows)
	{
		WindowInfo secWi = w.second.GetWindowInfo();
		if (secWi.TitleBar == 1 && secWi.X + secWi.Width - 125 <= x && x < secWi.X + secWi.Width - 85 && secWi.Y <= y && y <= secWi.Y + 30)
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
	WindowInfo wi = w.GetWindowInfo();
	if (wi.TitleBar == 1)
	{
		Element e = Element();
		e.properties.insert({ "Text", wi.Title });
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
		WindowInfo wi = windows[handle].GetWindowInfo();
		int pid = windows[handle].parentProcess;
		bool t = wi.TitleBar;
		windows.erase(handle);
		activeWindow = -1;
		for (pair<int, Window> w : windows)
		{
			WindowInfo secWi = w.second.GetWindowInfo();
			if (secWi.TitleBar)
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
		WindowInfo wi = windows[handle].GetWindowInfo();
		if (wi.Maximized) RestoreDown(handle);
		wi.X = x;
		wi.Y = y;
		windows[handle].SetWindowInfo(wi);
	}
	RenderWindows();
}
void WindowManager::ResizeWindow(int handle, int w, int h)
{
	if (windows.find(handle) != windows.end())
	{
		WindowInfo wi = windows[handle].GetWindowInfo();
		wi.Width = w;
		wi.Height = h;
		windows[handle].SetWindowInfo(wi);
	}
	RenderWindows();
}
void WindowManager::Maximize(int handle)
{
	if (windows.find(handle) != windows.end())
	{
		Window* w = &windows[handle];
		WindowInfo wi = w->GetWindowInfo();
		if (!wi.Maximized)
		{
			wi.PrevX = wi.X;
			wi.PrevY = wi.Y;
			wi.PrevWidth = wi.Width;
			wi.PrevHeight = wi.Height;
			wi.X = 0;
			wi.Y = 0;
			wi.Width = Graphics::resX - 1;
			wi.Height = Graphics::resY - 60;
			wi.Maximized = true;
			w->SetWindowInfo(wi);
		}
	}
	RenderWindows();
}
void WindowManager::RestoreDown(int handle)
{
	if (windows.find(handle) != windows.end())
	{
		Window* w = &windows[handle];
		WindowInfo wi = w->GetWindowInfo();
		if (wi.Maximized)
		{
			wi.X = wi.PrevX;
			wi.Y = wi.PrevY;
			wi.Width = wi.PrevWidth;
			wi.Height = wi.PrevHeight;
			wi.PrevX = 0;
			wi.PrevY = 0;
			wi.PrevWidth = Graphics::resX;
			wi.PrevHeight = Graphics::resY - 60;
			wi.Maximized = false;
			w->SetWindowInfo(wi);
		}
	}
	RenderWindows();
}
void WindowManager::HideWindow(int handle)
{
	if (windows.find(handle) != windows.end())
	{
		WindowInfo wi = windows[handle].GetWindowInfo();
		wi.Hidden = true;
		windows[handle].SetWindowInfo(wi);
		activeWindow = -1;
		for (pair<int, Window> w : windows)
		{
			WindowInfo secWi = w.second.GetWindowInfo();
			if (secWi.TitleBar && !secWi.Hidden)
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
	if (windows.find(handle) != windows.end())
	{
		WindowInfo wi = windows[handle].GetWindowInfo();
		wi.Hidden = false;
		windows[handle].SetWindowInfo(wi);
	}
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
void WindowManager::Stop()
{

}
void WindowManager::FireEvent(EffigyEvent evt, int p)
{

}
void WindowManager::FireEvent(EffigyEvent evt, int x, int y)
{
	if (evt == EffigyEvent::MouseDown)
	{
		activeWindow = GetWindowHandle(x, y);
		WindowInfo wi = windows[activeWindow].GetWindowInfo();
		if (activeWindow != -1)
		{
			if (Graphics::glareEnabled)
			{
				if (wi.TitleBar == 1)
				{
					if (GetWindowTitleBarHandle(x, y) == activeWindow)
						dragging = true;
					if (hoveringOnClose) CloseWindow(activeWindow);
					if (hoveringOnMaximize)
					{
						if (wi.Maximized) RestoreDown(activeWindow);
						else Maximize(activeWindow);
					}
					if (hoveringOnMinimize) HideWindow(activeWindow);
				}
			}
			else
			{
				if (wi.TitleBar == 1)
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
						if (windows[mxbh].GetWindowInfo().Maximized) RestoreDown(mxbh);
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
		int iw = GetWindowHandle(x, y);
		activeWindow = iw;
		Window& w = windows[iw];
		WindowInfo secWi = w.GetWindowInfo();
		for (Element& e : w.elements)
		{
			if (e.GetProperty("Type") == "Button")
			{
				Element* ew = &(windows[iw].elements[windows[iw].GetElementIndexByID(e.IGetProperty("ID"))]);
				if (e.Hover(x - secWi.X, y - secWi.Y - secWi.DrawableBase))
				{
					if (secWi.WakeOnInteraction)
					{
						//vmmgr::processes[secWi.parentProcess]->suspended = false;
					}
					if (ew->eventHandler != -1)
					{
						//vmmgr::processes[secWi.parentProcess]->branch(ew->eventHandler);
					}
				}
			}
			else if (e.GetProperty("Type") == "CheckBox")
			{
				Element* ew = &(windows[iw].elements[windows[iw].GetElementIndexByID(e.IGetProperty("ID"))]);
				if (e.Hover(x - secWi.X, y - secWi.Y - secWi.DrawableBase))
				{
					if (ew->IGetProperty("Checked") == 0) ew->properties["Checked"] = "1";
					else ew->properties["Checked"] = "0";
					if (secWi.WakeOnInteraction)
					{
						//vmmgr::processes[secWi.parentProcess]->suspended = false;
					}
					if (ew->eventHandler != -1)
					{
						//vmmgr::processes[secWi.parentProcess]->branch(ew->eventHandler);
					}
					RenderWindows();
				}
			}
			else if (e.GetProperty("Type") == "ListView")
			{
				Element* ew = &(windows[iw].elements[windows[iw].GetElementIndexByID(e.IGetProperty("ID"))]);
				if (e.HoverEx(x - secWi.X, y - secWi.Y - secWi.DrawableBase, w.GetWindowInfo().Width, w.GetWindowInfo().Height))
				{
					int pY = y - secWi.Y - secWi.DrawableBase;
					int ecunt = 0;
					string ck = "";
					string el = e.GetProperty("Items");
					bool truly;
					for (int i = 0; i < el.size(); i++)
					{
						truly = true;
						if (el[i] == ',')
						{
							if (i == 1)
							{
								if (el[0] == '\\')
									truly = false;
							}
							else if (i > 1)
							{
								if (el[i - 1] == '\\' && el[i - 2] != '\\')
									truly = false;
							}
							if (truly)
							{
								ecunt++;
								ck = "";
							}
						}
						else
						{
							truly = false;
						}
						if (!truly)
						{
							ck += el[i];
						}
					}
					int fs = ew->IGetProperty("FrameStart");
					int ih = TTF_FontHeight(Graphics::fonts[ew->GetProperty("Font")].font) + 2;
					int ww = e.IGetProperty("Width");
					int wh = e.IGetProperty("Height");
					if (ww <= 0)
					{
						ww = wi.Width;
					}
					if (wh <= 0)
					{
						wh = wi.Height;
					}
					int meos = wh / ih;
					int yp = 0;
					if (fs > 0)
					{
						if (pY > 0 && pY < ih)
						{
							if (fs < meos)
							{
								ew->SetProperty("FrameStart", 0);
							}
							else
							{
								ew->SetProperty("FrameStart", fs - meos + 2);
							}
							RenderWindows();
							break;
						}
						if(ecunt <= fs + meos) meos--;
						yp++;
					}
					if (ecunt > fs + meos)
					{
						if (pY > (meos - 1) * ih && pY < (meos - 1) * ih + ih)
						{
							if (ecunt >= fs + meos + meos)
							{
								ew->SetProperty("FrameStart", fs + meos - 1);
							}
							else
							{
								ew->SetProperty("FrameStart", ecunt - meos + 1);
							}
							RenderWindows();
							break;
						}
						meos--;
					}
					for (int i = fs; i < fs + meos; i++)
					{
						if (pY > yp * ih && pY < yp * ih + ih)
						{
							ew->SetProperty("SelectedIndex", i);
							break;
						}
						yp++;
					}
					RenderWindows();
				}
			}
		}
		for (Element e : taskbar)
		{
			if (Graphics::glareEnabled)
			{
				if (e.GetProperty("Hovering") == "1")
				{
					ShowWindow(e.IGetProperty("Window ID"));
					activeWindow = e.IGetProperty("Window ID");
				}
			}
			else
			{
				for (Element& e : taskbar)
				{
					if (e.Hover(x, y))
					{
						ShowWindow(e.IGetProperty("Window ID"));
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
			WindowInfo wi = windows[activeWindow].GetWindowInfo();
			int closeHandle = GetWindowCloseButtonHandle(x, y);
			int maxHandle = GetWindowMaximizeButtonHandle(x, y);
			int minHandle = GetWindowMinimizeButtonHandle(x, y);
			if (!wi.MaximizeButton) minHandle = GetWindowMaximizeButtonHandle(x, y);
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
					WindowInfo wi = w.GetWindowInfo();
					Graphics::SetColor(Theming::CloseButtonHoverColor);
					Graphics::DrawFilledRectangle(wi.X + wi.Width - 45, wi.Y + 1, 44, 29, Theming::CloseButtonHoverColor);
					Graphics::DrawImage(wi.X + wi.Width - 35, wi.Y + 6, "closelight");
					Graphics::UpdateScreen();
					hoveringOnClose = true;
				}
			}
			if (maxHandle != -1)
			{
				Window w = windows[maxHandle];
				WindowInfo wi = w.GetWindowInfo();
				if ((!hoveringOnMaximize && wi.MaximizeButton) || (!hoveringOnMinimize && !wi.MaximizeButton))
				{
					Graphics::SetColor(Theming::CaptionButtonHoverColor);
					Graphics::DrawFilledRectangle(wi.X + wi.Width - 85, wi.Y + 1, 39, 29, Theming::CaptionButtonHoverColor);
					if (wi.MaximizeButton)
					{
						if (wi.Maximized) Graphics::DrawImage(wi.X + wi.Width - 75, wi.Y + 6, "restore");
						else Graphics::DrawImage(wi.X + wi.Width - 75, wi.Y + 6, "maximize");
						hoveringOnMaximize = true;
					}
					else
					{
						Graphics::DrawImage(wi.X + wi.Width - 75, wi.Y + 5, "minimize");
						hoveringOnMinimize = true;
					}
					Graphics::UpdateScreen();
				}
			}
			if (minHandle != -1)
			{
				if (!hoveringOnMinimize)
				{
					Window w = windows[minHandle];
					WindowInfo wi = w.GetWindowInfo();
					if (wi.MaximizeButton)
					{
						Graphics::SetColor(Theming::CaptionButtonHoverColor);
						Graphics::DrawFilledRectangle(wi.X + wi.Width - 125, wi.Y + 1, 39, 29, Theming::CaptionButtonHoverColor);
						Graphics::DrawImage(wi.X + wi.Width - 115, wi.Y + 5, "minimize");
						Graphics::UpdateScreen();
						hoveringOnMinimize = true;
					}
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
							WindowInfo secWi = w.second.GetWindowInfo();
							if (e.Hover(x - secWi.X, y - secWi.Y - secWi.DrawableBase))
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
#endif