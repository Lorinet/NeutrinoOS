#include "viewmgr.h"

map<int, View> ViewManager::views;
int ViewManager::activeView;

void ViewManager::Initialize()
{
	Graphics::InitGraphicSystem();
	Graphics::fonts.insert({ "Micro 4", (FontType) { (uint8_t*)u8g2_font_u8glib_4_tf, 4 } });
	Graphics::fonts.insert({ "Micro 5", (FontType) { (uint8_t*)u8g2_font_micro_mr, 5 } });
	Graphics::fonts.insert({ "Console 7", (FontType) { (uint8_t*)u8g2_font_pxplusibmcga_8f, 7 } });
	Graphics::fonts.insert({ "Times New Roman 7", (FontType) { (uint8_t*)u8g2_font_timR08_tf, 7 } });
	Graphics::fonts.insert({ "Times New Roman 7 Bold", (FontType) { (uint8_t*)u8g2_font_timB08_tf, 7 } });
	Graphics::fonts.insert({ "Helvetica 8", (FontType) { (uint8_t*)u8g2_font_helvR08_tf, 8 } });
	Graphics::fonts.insert({ "Helvetica 8 Bold", (FontType) { (uint8_t*)u8g2_font_helvB08_tf, 8 } });
	Graphics::fonts.insert({ "Open Iconic 8", (FontType) { (uint8_t*)u8g2_font_open_iconic_all_1x_t, 8 } });
	Graphics::fonts.insert({ "Crox 9", (FontType) { (uint8_t*)u8g2_font_crox1h_tf, 9 } });
	Graphics::fonts.insert({ "Crox 9 Bold", (FontType) { (uint8_t*)u8g2_font_crox1hb_tf, 9 } });
	Graphics::fonts.insert({ "Profont 9", (FontType) { (uint8_t*)u8g2_font_profont15_mf, 9 } });
	Graphics::fonts.insert({ "Times New Roman 10", (FontType) { (uint8_t*)u8g2_font_timR10_tf, 10 } });
	Graphics::fonts.insert({ "Times New Roman 10 Bold", (FontType) { (uint8_t*)u8g2_font_timB10_tf, 10 } });
	Graphics::fonts.insert({ "Console 10", (FontType) { (uint8_t*)u8g2_font_pxplusibmvga9_tf, 10 } });
	Graphics::fonts.insert({ "Profont 11", (FontType) { (uint8_t*)u8g2_font_profont17_tf, 11 } });
	Graphics::fonts.insert({ "Courier New 11", (FontType) { (uint8_t*)u8g2_font_courB14_tf, 11 } });
	Graphics::fonts.insert({ "Helvetica 11", (FontType) { (uint8_t*)u8g2_font_helvR10_tf, 11 } });
	Graphics::fonts.insert({ "Helvetica 11 Bold", (FontType) { (uint8_t*)u8g2_font_helvB10_tf, 11 } });
	Graphics::fonts.insert({ "Helvetica 12", (FontType) { (uint8_t*)u8g2_font_helvB12_tf, 12 } });
	Graphics::fonts.insert({ "Helvetica 12 Thin", (FontType) { (uint8_t*)u8g2_font_helvR12_tf, 12 } });
	Graphics::fonts.insert({ "Times New Roman 13", (FontType) { (uint8_t*)u8g2_font_timR14_tf, 13 } });
	Graphics::fonts.insert({ "Times New Roman 13 Bold", (FontType) { (uint8_t*)u8g2_font_timB14_tf, 13 } });
	Graphics::fonts.insert({ "Helvetica 14", (FontType) { (uint8_t*)u8g2_font_helvR14_tf, 14 } });
	Graphics::fonts.insert({ "Inconsolata 16", (FontType) { (uint8_t*)u8g2_font_inr16_mf, 16 } });
	Graphics::fonts.insert({ "Logisoso 16", (FontType) { (uint8_t*)u8g2_font_logisoso16_tf, 16 } });
	Graphics::fonts.insert({ "Open Iconic 16", (FontType) { (uint8_t*)u8g2_font_open_iconic_all_2x_t, 16 } });
	Graphics::fonts.insert({ "Courier New 19", (FontType) { (uint8_t*)u8g2_font_courR24_tf, 19 } });
	Graphics::fonts.insert({ "Helvetica 19", (FontType) { (uint8_t*)u8g2_font_helvR18_tf, 19 } });
	Graphics::fonts.insert({ "Inconsolata 19", (FontType) { (uint8_t*)u8g2_font_inr19_mf, 19 } });
	Graphics::fonts.insert({ "Battery 19", (FontType) { (uint8_t*)u8g2_font_battery19_tn, 19 } });
	Graphics::fonts.insert({ "Logisoso 20", (FontType) { (uint8_t*)u8g2_font_logisoso20_tf, 20 } });
	Graphics::fonts.insert({ "Logisoso 22", (FontType) { (uint8_t*)u8g2_font_logisoso22_tf, 22 } });
	Graphics::fonts.insert({ "Times New Roman 23", (FontType) { (uint8_t*)u8g2_font_timR24_tf, 23 } });
	Graphics::fonts.insert({ "Helvetica 25", (FontType) { (uint8_t*)u8g2_font_helvR24_tf, 25 } });
	Graphics::fonts.insert({ "Helvetica 25 Bold", (FontType) { (uint8_t*)u8g2_font_helvB24_tf, 25 } });
	Graphics::fonts.insert({ "Freedoom 26 Numbers", (FontType) { (uint8_t*)u8g2_font_freedoomr25_tn, 26 } });
	Graphics::fonts.insert({ "Logisoso 26", (FontType) { (uint8_t*)u8g2_font_logisoso26_tf, 26 } });
	Graphics::fonts.insert({ "Logisoso 28", (FontType) { (uint8_t*)u8g2_font_logisoso28_tf, 28 } });
	Graphics::fonts.insert({ "Logisoso 32", (FontType) { (uint8_t*)u8g2_font_logisoso32_tf, 32 } });
	Graphics::fonts.insert({ "Open Iconic 32", (FontType) { (uint8_t*)u8g2_font_open_iconic_all_4x_t, 32 } });
	Graphics::fonts.insert({ "Logisoso 38", (FontType) { (uint8_t*)u8g2_font_logisoso38_tf, 38 } });
	Graphics::fonts.insert({ "Segments 42 Numbers", (FontType) { (uint8_t*)u8g2_font_7Segments_26x42_mn, 42 } });
	Graphics::fonts.insert({ "Open Iconic 48", (FontType) { (uint8_t*)u8g2_font_open_iconic_all_6x_t, 48 } });
	Graphics::fonts.insert({ "Open Iconic 64", (FontType) { (uint8_t*)u8g2_font_open_iconic_all_8x_t , 64 } });
	Graphics::ClearScreen();
}

void ViewManager::RenderView(View v)
{
	Graphics::ClearScreen();
	for (int i = 0; i < v.elements.size(); i++)
	{
		Element e = v.elements[i];
		if (e.GetProperty("Type") == "Label")
		{
			int fs = Graphics::fonts[e.GetProperty("Font")].height;
			Graphics::SetPos(e.GetPropertyInt("Position X"), e.GetPropertyInt("Position Y") + fs);
			Graphics::DrawString(e.GetProperty("Text"), Graphics::fonts[e.GetProperty("Font")]);
		}
		else if (e.GetProperty("Type") == "Button")
		{
			Graphics::SetPos(e.GetPropertyInt("Position X"), e.GetPropertyInt("Position Y"));
			Graphics::SetSize(e.GetPropertyInt("Width"), e.GetPropertyInt("Height"));
			if (Graphics::width == 0) Graphics::width = 6 + Graphics::GetTextWidth(e.GetProperty("Text"), Graphics::fonts[e.GetProperty("Font")]);
			if (Graphics::height == 0) Graphics::height = Graphics::fonts[e.GetProperty("Font")].height + 7;
			if (i == v.focusedElement)
			{
				Graphics::SetColor(2);
				Graphics::DrawFilledRectangle(1);
				Graphics::posX += 3;
				Graphics::posY += Graphics::fonts[e.GetProperty("Font")].height + 3;
				Graphics::DrawString(e.properties["Text"], Graphics::fonts[e.GetProperty("Font")]);
				Graphics::SetColor(1);
			}
			else
			{
				Graphics::DrawRectangle();
				Graphics::posX += 3;
				Graphics::posY += 11;
				Graphics::DrawString(e.properties["Text"], Graphics::fonts[e.GetProperty("Font")]);
			}
		}
		else if (e.GetProperty("Type") == "CheckBox")
		{
			Graphics::SetPos(e.GetPropertyInt("Position X"), e.GetPropertyInt("Position Y"));
			Graphics::SetSize(9, 9);
			if (i == v.focusedElement)
			{
				Graphics::DrawFilledRectangle(1);
				if (e.GetPropertyInt("Checked") == 1)
				{
					Graphics::posX += 2;
					Graphics::posY += 2;
					Graphics::SetSize(5, 5);
					Graphics::SetColor(0);
					Graphics::DrawFilledRectangle(0);
					Graphics::SetColor(1);
				}
			}
			else
			{
				Graphics::DrawRectangle();
				if (e.GetPropertyInt("Checked") == 1)
				{
					Graphics::posX += 3;
					Graphics::posY += 3;
					Graphics::SetSize(3, 3);
					Graphics::DrawFilledRectangle(1);
				}
			}
			Graphics::SetPos(e.GetPropertyInt("Position X") + 12, e.GetPropertyInt("Position Y") + Graphics::fonts[e.GetProperty("Font")].height);
			Graphics::DrawString(e.properties["Text"], Graphics::fonts[e.GetProperty("Font")]);
		}
	}
	Graphics::UpdateScreen();
}
int ViewManager::CreateView(View v)
{
	int index = 0;
	while (views.find(index) != views.end()) index++;
	views.insert({ index, v });
	activeView = index;
	RenderView(views[activeView]);
	return index;
}
void ViewManager::CloseView(int handle)
{
	if (views.find(handle) != views.end())
	{
		views.erase(handle);
		activeView = 0;
		RenderView(views[activeView]);
	}
}
void ViewManager::FireEvent(TiwazEvent e)
{
	if (e == TiwazEvent::SelectButton)
	{
		View* v = &views[activeView];
		if (v->ContainsSelectables())
		{
			bool got = false;
			while (!got)
			{
				v->focusedElement += 1;
				if (v->focusedElement > v->GetLastIndex()) v->focusedElement = 0;
				if (v->GetElementByID(v->focusedElement)->GetProperty("Selectable") == "1") got = true;
			}
			RenderView(views[activeView]);
		}
	}
	else if (e == TiwazEvent::EnterButton)
	{
		if (views[activeView].focusedElement != -1)
		{
			Element* elm = &views[activeView].elements[views[activeView].focusedElement];
			if (elm->GetProperty("Type") == "CheckBox")
			{
				if (elm->GetProperty("Checked") == "1") elm->properties["Checked"] = "0";
				else elm->properties["Checked"] = "1";
				RenderView(views[activeView]);
				if (elm->eventHandler != -1)
					vmmgr::processes[views[activeView].parentProcess].branch(elm->eventHandler);
			}
			else if (elm->GetProperty("Type") == "Button")
			{
				if (elm->eventHandler != -1)
					vmmgr::processes[views[activeView].parentProcess].branch(elm->eventHandler);
			}
		}
	}
}