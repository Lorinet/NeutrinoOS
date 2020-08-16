#pragma once
#include <cstdint>
#include <map>
#include <string>
#include "platformconfig.h"
#if defined(__WIN32)
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#elif defined(__UNIX)
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#endif
#include "SDL_gfxPrimitives.h"
using namespace std;
struct Color
{
public:
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t A;
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};
struct Texture
{
public:
	SDL_Surface* texture;
	int width;
	int height;
};

struct FontType
{
public:
	TTF_Font* font;
};
class Graphics
{
public:
	static const int resX = 1280;
	static const int resY = 720;
	static Color color;
	static bool glareEnabled;
	static void InitGraphicSystem();
	static void ClearScreen();
	static void UpdateScreen();
	static void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	static void SetColor(Color c);
	static void DrawPixel(int x, int y);
	static void DrawRectangle(int x, int y, int w, int h);
	static void DrawFilledRectangle(int x, int y, int w, int h, Color fill);
	static void DrawString(int x, int y, string text, string font);
	static void LoadImageE(string path, string id);
	static void LoadFont(string path, string id, int size);
	static void DrawImage(int x, int y, string id);
	static void DrawLine(int x1, int y1, int x2, int y2);
	static map<string, FontType> fonts;
	static map<string, Texture> textures;
	static SDL_Surface* window;
};
