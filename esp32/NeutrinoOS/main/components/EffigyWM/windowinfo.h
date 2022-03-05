#include "window.h"
struct WindowInfo
{
	int X = Graphics::resX / 3;
	int Y = Graphics::resY / 3;
	int PrevX = X;
	int PrevY = Y;
	int Width = 150;
	int Height = 120;
	int PrevWidth = Width;
	int PrevHeight = Height;
	int DrawableBase = 30;
	int WindowID = -1;
	string Title = "";
	bool TitleBar = true;
	bool MaximizeButton = true;
	bool StickyDraw = false;
	bool WakeOnInteraction = false;
	bool Hidden = false;
	bool Maximized = false;
	WindowInfo(Window* w)
	{
		Element* windowinfo = w->GetElementByID(0);
		if (windowinfo != NULL)
		{
			if (windowinfo->GetProperty("Type") == "WindowInfo")
			{
				Title = windowinfo->GetProperty("Title");
				if (windowinfo->IGetProperty("TitleBar") == 0) TitleBar = false;
				if (windowinfo->IGetProperty("MaximizeButton") == 0) MaximizeButton = false;
				if (windowinfo->IGetProperty("StickyDraw") == 1) StickyDraw = true;
				if (windowinfo->IGetProperty("WakeOnInteraction") == 1) WakeOnInteraction = true;
				if (windowinfo->IGetProperty("Position X") != -1) X = windowinfo->IGetProperty("Position X");
				if (windowinfo->IGetProperty("Position Y") != -1) Y = windowinfo->IGetProperty("Position Y");
				if (windowinfo->IGetProperty("Width") != -1) Width = windowinfo->IGetProperty("Width");
				if (windowinfo->IGetProperty("Height") != -1) Height = windowinfo->IGetProperty("Height");
				if (windowinfo->IGetProperty("Hidden") == 1) Hidden = true;
				if (windowinfo->IGetProperty("Maximized") == 1) Maximized = true;
				if (windowinfo->IGetProperty("Prev X") != -1) PrevX = windowinfo->IGetProperty("Prev X");
				if (windowinfo->IGetProperty("Prev Y") != -1) PrevY = windowinfo->IGetProperty("Prev Y");
				if (windowinfo->IGetProperty("Prev Width") != -1) PrevWidth = windowinfo->IGetProperty("Prev Width");
				if (windowinfo->IGetProperty("Prev Height") != -1) PrevHeight = windowinfo->IGetProperty("Prev Height");
				DrawableBase = TitleBar * 30;
			}
		}
	}
};