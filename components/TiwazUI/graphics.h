#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <iostream>
#include "bitmaps.h"
#include "util.h"
#if defined(__UNIX)
#include "U8g2lib.h"
#elif defined(__ESP32)
#include "u8g2/u8g2.h"
#include "u8g2_esp32_hal.h"
#endif
#include "memorystats.h"


using namespace std;

class FontType
{
public:
	uint8_t* data;
	int height;
};
class GraphicalTexture
{
public:
	uint8_t* data;
};
class Graphics
{
public:
	static u8g2_t u8g2;
	static const int resX = 128;
	static const int resY = 64;
	static int posX;
	static int posY;
	static int width;
	static int height;
	static int thickness;
	static int color;
	static bool running;
	static void InitGraphicSystem();
	static void ClearScreen();
	static void SetPos(int x, int y);
	static void SetSize(int w, int h);
	static void SetThickness(int thick);
	static void SetColor(int c);
	static void DrawPixel();
	static void DrawRectangle();
	static void DrawFilledRectangle(int fill);
	static int GetTextWidth(string text, FontType font);
	static void DrawString(string text, FontType font);
	static void DrawStringWordWrap(string text, FontType font);
	static void DrawStringWordWrapEx(string text, FontType font, int& curx, int& cury);
	static void LoadImage(string path, string id);
	static void LoadFont(string path, string id);
	static void DrawImage(string id);
	static void UpdateScreen();
	// use width and height as dest pos
	static void DrawLine();
	static map<string, FontType> fonts;
	static map<string, GraphicalTexture> textures;
};

