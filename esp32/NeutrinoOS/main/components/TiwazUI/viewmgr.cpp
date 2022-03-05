#include "viewmgr.h"

#ifdef COMPONENT_TIWAZ
#include "nvm.h"
#include "kernlog.h"
#include "graphics.h"
#include "vmmgr.h"
#include "memorystats.h"
#include "bitconverter.h"

winmgr_api winmgr_api::instance;

void winmgr_api::initialize()
{
	instance = winmgr_api();
	ViewManager::Initialize();
	inputmgr::initialize();
}

Array<byte> winmgr_api::message(Array<byte> indata, nvm* v)
{
	int id = 0;
	int id2 = 0;
	int proc = 0;
	bool bl = false;
	string txt = "";
	string txt1 = "";
	string txt2 = "";
	switch (indata[0])
	{
	case uicmd::CreateView:
		id = ViewManager::CreateView(UISerialization::DeserializeView(bitconverter::tostring(indata, 1)));
		ViewManager::views[id]->parentProcess = v->processid;
		return bitconverter::toArray(id);
		break;
	case uicmd::DestroyView:
		ViewManager::CloseView(bitconverter::toint32(indata, 1));
		break;
	case uicmd::AddElement:
		id = bitconverter::toint32(indata, 1);
		txt = bitconverter::tostring(indata, 5);
		ViewManager::views[id]->elements.push(UISerialization::DeserializeElement(txt));
		ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
		break;
	case uicmd::ModifyElement:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		txt = bitconverter::tostring(indata, 9);
		*ViewManager::views[id]->GetElementByID(id2) = UISerialization::DeserializeElement(txt);
		ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
		break;
	case uicmd::DeleteElement:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		ViewManager::views[id]->elements.removeAt(ViewManager::views[id]->GetElementIndexByID(id2));
		ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
		break;
	case uicmd::GetPropertyValue:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		txt = bitconverter::tostring(indata, 9);
		return bitconverter::toArray(ViewManager::views[id]->elements[ViewManager::views[id]->GetElementIndexByID(id2)].GetProperty(txt));
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
		ViewManager::views[id]->elements[ViewManager::views[id]->GetElementIndexByID(id2)].properties[txt1] = util::replaceAll(txt2, "\\:", ":");
		ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
		break;
	case uicmd::SwitchView:
		id = bitconverter::toint32(indata, 1);
		ViewManager::activeView = id;
		ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
		break;
	case uicmd::UpdateScreen:
		ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
		break;
	case uicmd::AttachEventHandler:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		proc = bitconverter::toint32(indata, 9);
		ViewManager::views[id]->elements[ViewManager::views[id]->GetElementIndexByID(id2)].eventHandler = proc;
		break;
	case uicmd::DetachEventHandler:
		id = bitconverter::toint32(indata, 1);
		id2 = bitconverter::toint32(indata, 5);
		ViewManager::views[id]->elements[ViewManager::views[id]->GetElementIndexByID(id2)].eventHandler = -1;
		break;
	}
	return Array<byte>();
}

map<int, View *> ViewManager::views;
int ViewManager::activeView;

void ViewManager::Initialize()
{
	klog("TiwazViewManager", "Initializing...");
	Graphics::InitGraphicSystem();
	views = map<int, View *>();
	klog("TiwazViewManager", "Loading resources...");
	Graphics::fonts.emplace("Micro 4", (FontType){(uint8_t *)u8g2_font_u8glib_4_tf, 4});
	Graphics::fonts.emplace("Micro 5", (FontType){(uint8_t *)u8g2_font_micro_mr, 5});
	Graphics::fonts.emplace("Console 7", (FontType){(uint8_t *)u8g2_font_pxplusibmcga_8f, 7});
	Graphics::fonts.emplace("Times New Roman 7", (FontType){(uint8_t *)u8g2_font_timR08_tf, 7});
	Graphics::fonts.emplace("Times New Roman 7 Bold", (FontType){(uint8_t *)u8g2_font_timB08_tf, 7});
	Graphics::fonts.emplace("Helvetica 8", (FontType){(uint8_t *)u8g2_font_helvR08_tf, 8});
	Graphics::fonts.emplace("Helvetica 8 Bold", (FontType){(uint8_t *)u8g2_font_helvB08_tf, 8});
	Graphics::fonts.emplace("Open Iconic 8", (FontType){(uint8_t *)u8g2_font_open_iconic_all_1x_t, 8});
	Graphics::fonts.emplace("Crox 9", (FontType){(uint8_t *)u8g2_font_crox1h_tf, 9});
	Graphics::fonts.emplace("Crox 9 Bold", (FontType){(uint8_t *)u8g2_font_crox1hb_tf, 9});
	Graphics::fonts.emplace("Profont 9", (FontType){(uint8_t *)u8g2_font_profont15_mf, 9});
	Graphics::fonts.emplace("Times New Roman 10", (FontType){(uint8_t *)u8g2_font_timR10_tf, 10});
	Graphics::fonts.emplace("Times New Roman 10 Bold", (FontType){(uint8_t *)u8g2_font_timB10_tf, 10});
	Graphics::fonts.emplace("Console 10", (FontType){(uint8_t *)u8g2_font_pxplusibmvga9_tf, 10});
	Graphics::fonts.emplace("Profont 11", (FontType){(uint8_t *)u8g2_font_profont17_tf, 11});
	Graphics::fonts.emplace("Courier New 11", (FontType){(uint8_t *)u8g2_font_courB14_tf, 11});
	Graphics::fonts.emplace("Helvetica 11", (FontType){(uint8_t *)u8g2_font_helvR10_tf, 11});
	Graphics::fonts.emplace("Helvetica 11 Bold", (FontType){(uint8_t *)u8g2_font_helvB10_tf, 11});
	Graphics::fonts.emplace("Helvetica 12", (FontType){(uint8_t *)u8g2_font_helvB12_tf, 12});
	Graphics::fonts.emplace("Helvetica 12 Thin", (FontType){(uint8_t *)u8g2_font_helvR12_tf, 12});
	Graphics::fonts.emplace("Times New Roman 13", (FontType){(uint8_t *)u8g2_font_timR14_tf, 13});
	Graphics::fonts.emplace("Times New Roman 13 Bold", (FontType){(uint8_t *)u8g2_font_timB14_tf, 13});
	Graphics::fonts.emplace("Helvetica 14", (FontType){(uint8_t *)u8g2_font_helvR14_tf, 14});
	Graphics::fonts.emplace("Inconsolata 16", (FontType){(uint8_t *)u8g2_font_inr16_mf, 16});
	Graphics::fonts.emplace("Logisoso 16", (FontType){(uint8_t *)u8g2_font_logisoso16_tf, 16});
	Graphics::fonts.emplace("Open Iconic 16", (FontType){(uint8_t *)u8g2_font_open_iconic_all_2x_t, 16});
	Graphics::fonts.emplace("Courier New 19", (FontType){(uint8_t *)u8g2_font_courR24_tf, 19});
	Graphics::fonts.emplace("Helvetica 19", (FontType){(uint8_t *)u8g2_font_helvR18_tf, 19});
	Graphics::fonts.emplace("Inconsolata 19", (FontType){(uint8_t *)u8g2_font_inr19_mf, 19});
	Graphics::fonts.emplace("Battery 19", (FontType){(uint8_t *)u8g2_font_battery19_tn, 19});
	Graphics::fonts.emplace("Logisoso 20", (FontType){(uint8_t *)u8g2_font_logisoso20_tf, 20});
	Graphics::fonts.emplace("Logisoso 22", (FontType){(uint8_t *)u8g2_font_logisoso22_tf, 22});
	Graphics::fonts.emplace("Times New Roman 23", (FontType){(uint8_t *)u8g2_font_timR24_tf, 23});
	Graphics::fonts.emplace("Helvetica 25", (FontType){(uint8_t *)u8g2_font_helvR24_tf, 25});
	Graphics::fonts.emplace("Helvetica 25 Bold", (FontType){(uint8_t *)u8g2_font_helvB24_tf, 25});
	Graphics::fonts.emplace("Freedoom 26 Numbers", (FontType){(uint8_t *)u8g2_font_freedoomr25_tn, 26});
	Graphics::fonts.emplace("Logisoso 26", (FontType){(uint8_t *)u8g2_font_logisoso26_tf, 26});
	Graphics::fonts.emplace("Logisoso 28", (FontType){(uint8_t *)u8g2_font_logisoso28_tf, 28});
	Graphics::fonts.emplace("Logisoso 32", (FontType){(uint8_t *)u8g2_font_logisoso32_tf, 32});
	Graphics::fonts.emplace("Open Iconic 32", (FontType){(uint8_t *)u8g2_font_open_iconic_all_4x_t, 32});
	Graphics::fonts.emplace("Logisoso 38", (FontType){(uint8_t *)u8g2_font_logisoso38_tf, 38});
	Graphics::fonts.emplace("Segments 42 Numbers", (FontType){(uint8_t *)u8g2_font_7Segments_26x42_mn, 42});
	Graphics::fonts.emplace("Open Iconic 48", (FontType){(uint8_t *)u8g2_font_open_iconic_all_6x_t, 48});
	Graphics::fonts.emplace("Open Iconic 64", (FontType){(uint8_t *)u8g2_font_open_iconic_all_8x_t, 64});
	Graphics::ClearScreen();
	klog("TiwazWindowManager", "Loaded");
}

void ViewManager::RenderView(View *v)
{
	if (!v->properties.contains("StickyDraw"))
		Graphics::ClearScreen();
	Element *e;
	int fs;
	for (int i = 0; i < v->elements.size; i++)
	{
		e = &v->elements[i];
		if (e->GetPropertyInt("Hidden") == 1)
			continue;
		if (e->GetProperty("Type") == "Label")
		{
			fs = Graphics::fonts[e->GetProperty("Font")].height;
			Graphics::SetPos(e->GetPropertyInt("Position X"), e->GetPropertyInt("Position Y") + fs);
			Graphics::DrawString(e->GetProperty("Text"), Graphics::fonts[e->GetProperty("Font")]);
		}
		else if (e->GetProperty("Type") == "Button")
		{
			Graphics::SetPos(e->GetPropertyInt("Position X"), e->GetPropertyInt("Position Y"));
			Graphics::SetSize(e->GetPropertyInt("Width"), e->GetPropertyInt("Height"));
			if (Graphics::width == 0)
				Graphics::width = 6 + Graphics::GetTextWidth(e->GetProperty("Text"), Graphics::fonts[e->GetProperty("Font")]);
			if (Graphics::height == 0)
				Graphics::height = Graphics::fonts[e->GetProperty("Font")].height + 7;
			if (i == v->focusedElement)
			{
				Graphics::SetColor(2);
				Graphics::DrawFilledRectangle(1);
				Graphics::posX += 3;
				Graphics::posY += Graphics::fonts[e->GetProperty("Font")].height + 3;
				Graphics::DrawString(e->properties["Text"], Graphics::fonts[e->GetProperty("Font")]);
				Graphics::SetColor(1);
			}
			else
			{
				Graphics::DrawRectangle();
				Graphics::posX += 3;
				Graphics::posY += Graphics::fonts[e->GetProperty("Font")].height + 3;
				Graphics::DrawString(e->properties["Text"], Graphics::fonts[e->GetProperty("Font")]);
			}
		}
		else if (e->GetProperty("Type") == "CheckBox")
		{
			Graphics::SetPos(e->GetPropertyInt("Position X"), e->GetPropertyInt("Position Y"));
			Graphics::SetSize(9, 9);
			if (i == v->focusedElement)
			{
				Graphics::DrawFilledRectangle(1);
				if (e->GetPropertyInt("Checked") == 1)
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
				if (e->GetPropertyInt("Checked") == 1)
				{
					Graphics::posX += 3;
					Graphics::posY += 3;
					Graphics::SetSize(3, 3);
					Graphics::DrawFilledRectangle(1);
				}
			}
			Graphics::SetPos(e->GetPropertyInt("Position X") + 12, e->GetPropertyInt("Position Y") + Graphics::fonts[e->GetProperty("Font")].height);
			Graphics::DrawString(e->properties["Text"], Graphics::fonts[e->GetProperty("Font")]);
		}
		else if (e->GetProperty("Type") == "TextField")
		{
			Graphics::SetPos(e->GetPropertyInt("Position X"), e->GetPropertyInt("Position Y"));
			Graphics::SetSize(e->GetPropertyInt("Width"), e->GetPropertyInt("Height"));
			if (Graphics::width == 0)
			{
				e->properties["Width"] = to_string(Graphics::resX);
				Graphics::width = Graphics::resX;
			}
			if (Graphics::height == 0)
			{
				e->properties["Height"] = to_string(Graphics::resY);
				Graphics::height = Graphics::resY;
			}
			Graphics::DrawStringWordWrap(e->GetProperty("Text"), Graphics::fonts[e->GetProperty("Font")]);
			if (e->GetPropertyInt("Border") == 1)
			{
				Graphics::SetPos(e->GetPropertyInt("Position X"), e->GetPropertyInt("Position Y"));
				Graphics::SetSize(e->GetPropertyInt("Width") + 8, e->GetPropertyInt("Height") + 8);
				Graphics::DrawRectangle();
			}
		}
		else if (e->GetProperty("Type") == "TextBuffer")
		{
			Graphics::SetPos(e->GetPropertyInt("Cursor X"), e->GetPropertyInt("Cursor Y"));
			int curx = 0;
			int cury = 0;
			Graphics::DrawStringWordWrapEx(e->GetProperty("Text"), Graphics::fonts[e->GetProperty("Font")], curx, cury);
			e->SetPropertyInt("Cursor X", curx);
			e->SetPropertyInt("Cursor Y", cury);
		}
		else if (e->GetProperty("Type") == "ListView")
		{
			Array<string> keys = Array<string>();
			string ck = "";
			string el = e->GetProperty("Items");
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
			e->SetPropertyInt("Length", keys.size);
			Graphics::SetPos(2, 0);
			Graphics::SetColor(2);
			if(keys.size > 0)
			{
				for (int i = e->GetPropertyInt("FrameStart"); i < keys.size; i++)
				{
					if(e->GetPropertyInt("SelectedIndex") == i)
					{
						Graphics::SetSize(Graphics::resX - 3, Graphics::fonts[e->GetProperty("Font")].height + 4);
						Graphics::SetPos(0, Graphics::posY);
						Graphics::DrawFilledRectangle(2);
						Graphics::SetPos(2, Graphics::posY);
					}
					Graphics::posY += Graphics::fonts[e->GetProperty("Font")].height + 2;
					Graphics::DrawString(util::replaceAll(keys[i], "\\\\", "\\"), Graphics::fonts[e->GetProperty("Font")]);
					if(Graphics::posY >= Graphics::resY) break;
				}
				keys.clear();
			}
		}
	}
	Graphics::UpdateScreen();
}
int ViewManager::CreateView(View *v)
{
	int index = 0;
	while (views.find(index) != views.end())
		index++;
	views.emplace(index, v);
	activeView = index;
	RenderView(views[activeView]);
	return index;
}
void ViewManager::CloseView(int handle)
{
	if (views.find(handle) != views.end())
	{
		View *v = views[handle];
		views.erase(handle);
		delete v;
		for (map<int, View *>::iterator it = views.begin(); it != views.end(); it++)
		{
			activeView = it->first;
			RenderView(views[activeView]);
			return;
		}
		activeView = -1;
		Graphics::ClearScreen();
		Graphics::UpdateScreen();
	}
}
void ViewManager::FireEvent(TiwazEvent e)
{
	if (views.size() == 0)
		return;
	View *v;
	Element *elm;
	bool got;
	if (e == TiwazEvent::SelectButton)
	{
		v = views[activeView];
		if (v->properties.contains("WakeOnInteraction"))
		{
			vmmgr::processes[v->parentProcess]->suspended = false;
		}
		if (v->ContainsSelectables())
		{
			got = false;
			while (!got)
			{
				v->focusedElement += 1;
				if (v->focusedElement > v->GetLastIndex())
					v->focusedElement = 0;
				if (v->GetElementByID(v->focusedElement)->GetProperty("Selectable") == "1")
					got = true;
			}
			RenderView(views[activeView]);
		}
		else if(v->properties.contains("ListView"))
		{
			for(int i = 0; i < v->elements.size; i++)
			{
				if(v->elements.get(i).GetProperty("Type") == "ListView")
				{
					v->elements.get(i).SetPropertyInt("SelectedIndex", v->elements.get(i).GetPropertyInt("SelectedIndex") + 1);
					if(v->elements.get(i).GetPropertyInt("SelectedIndex") >= v->elements.get(i).GetPropertyInt("Length"))
					{
						v->elements.get(i).SetPropertyInt("SelectedIndex", 0);
						v->elements.get(i).SetPropertyInt("FrameStart", 0);
					}
					int maxn = Graphics::resY / (Graphics::fonts[v->elements.get(i).GetProperty("Font")].height + 4);
					if(v->elements.get(i).GetPropertyInt("SelectedIndex") > maxn)
					{
						v->elements.get(i).SetPropertyInt("FrameStart", v->elements.get(i).GetPropertyInt("SelectedIndex") - maxn);
					}
				}
			}
			RenderView(views[activeView]);
		}
	}
	else if (e == TiwazEvent::EnterButton)
	{
		v = views[activeView];
		if (views[activeView]->focusedElement != -1)
		{
			if (views[activeView]->properties.contains("WakeOnInteraction"))
			{
				vmmgr::processes[views[activeView]->parentProcess]->suspended = false;
			}
			elm = &views[activeView]->elements[views[activeView]->focusedElement];
			if (elm->GetProperty("Type") == "CheckBox")
			{
				if (elm->GetProperty("Checked") == "1")
					elm->properties["Checked"] = "0";
				else
					elm->properties["Checked"] = "1";
				RenderView(views[activeView]);
				if (elm->eventHandler != -1)
				{
					vmmgr::processes[views[activeView]->parentProcess]->branch(elm->eventHandler);
				}
			}
			else if (elm->GetProperty("Type") == "Button")
			{
				if (elm->eventHandler != -1)
				{
					vmmgr::processes[views[activeView]->parentProcess]->branch(elm->eventHandler);
				}
			}
		}
		else if(v->properties.contains("ListView"))
		{
			if (views[activeView]->properties.contains("WakeOnInteraction"))
			{
				vmmgr::processes[views[activeView]->parentProcess]->suspended = false;
			}
			for(int i = 0; i < v->elements.size; i++)
			{
				if(v->elements[i].GetProperty("Type") == "ListView")
				{
					if (v->elements.get(i).eventHandler != -1)
					{
						Array<string> keys = Array<string>();
						string ck = "";
						string el = v->elements.get(i).GetProperty("Items");
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
						//vmmgr::processes[views[activeView]->parentProcess]->astack.push(bitconverter::toArray(util::replaceAll(keys[v->elements.get(i).GetPropertyInt("SelectedIndex")], "\\\\", "\\")));
						//vmmgr::processes[views[activeView]->parentProcess]->branch(v->elements.get(i).eventHandler);
					}
				}
			}
		}
	}
}

#endif