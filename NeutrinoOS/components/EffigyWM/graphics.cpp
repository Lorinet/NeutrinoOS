#include "graphics.h"
extern "C" 
{
	FILE __iob_func[3] = { *stdin,*stdout,*stderr };
}
bool Graphics::glareEnabled = true;
SDL_Surface* Graphics::window;
Color Graphics::color = Color(0, 0, 0, 0);
map<string, FontType> Graphics::fonts;
map<string, Texture> Graphics::textures;
void Graphics::InitGraphicSystem()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_WM_SetCaption("Neutrino", "Neutrino");
	window = SDL_SetVideoMode(resX, resY, 24, 0);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);
	ClearScreen();
	UpdateScreen();
}
void Graphics::ClearScreen()
{
	SDL_FillRect(window, NULL, SDL_MapRGB(window->format, color.R, color.G, color.B));
}
void Graphics::UpdateScreen()
{
	SDL_UpdateRect(window, 0, 0, 0, 0);
}
void Graphics::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	color = Color(r, g, b, a);
}
void Graphics::SetColor(Color c)
{
	color = c;
}
void Graphics::DrawPixel(int x, int y)
{
	pixelColor(window, x, y, SDL_MapRGB(window->format, color.R, color.G, color.B));
}
void Graphics::DrawRectangle(int x, int y, int w, int h)
{
	rectangleRGBA(window, x, y, x + w, y + h, color.R, color.G, color.B, color.A);
}
void Graphics::DrawFilledRectangle(int x, int y, int w, int h, Color fill)
{
	rectangleRGBA(window, x, y, x + w, y + h, color.R, color.G, color.B, color.A);
	if (glareEnabled)
	{
		boxRGBA(window, x + 1, y + 1, x + 1 + w - 2, y + 1 + h - 2, fill.R, fill.G, fill.B, fill.A);
	}
	else
	{
		SDL_Rect sr;
		sr.x = x + 1;
		sr.y = y + 1;
		sr.w = w - 1;
		sr.h = h - 1;
		SDL_FillRect(window, &sr, SDL_MapRGB(window->format, fill.R, fill.G, fill.B));
	}
}
void Graphics::DrawString(int x, int y, string text, string font)
{
	if (fonts.find(font) == fonts.end()) return;
	SDL_Color sc;
	sc.r = color.R;
	sc.g = color.G;
	sc.b = color.B;
	SDL_Surface* ts = TTF_RenderText_Solid(fonts[font].font, text.c_str(), sc);
	int w, h;
	TTF_SizeText(fonts[font].font, text.c_str(), &w, &h);
	SDL_Rect sr;
	sr.x = x;
	sr.y = y;
	sr.w = w;
	sr.h = h;
	SDL_BlitSurface(ts, NULL, window, &sr);
	SDL_FreeSurface(ts);
}
void Graphics::LoadImageE(string path, string id)
{
	SDL_Surface* ts = IMG_Load(path.c_str());
	Texture tex;
	tex.texture = ts;
	tex.width = ts->w;
	tex.height = ts->h;
	textures.insert({ id, tex });
}
void Graphics::LoadFont(string path, string id, int size)
{
	fonts.insert({ id, { TTF_OpenFont(path.c_str(), size) } });
}
void Graphics::DrawImage(int x, int y, string id)
{
	SDL_Rect sr;
	sr.x = x;
	sr.y = y;
	sr.w = textures[id].width;
	sr.h = textures[id].height;
	SDL_BlitSurface(textures[id].texture, NULL, window, &sr);
}
void Graphics::DrawLine(int x1, int y1, int x2, int y2)
{
	lineColor(window, x1, y1, x2, y2, SDL_MapRGB(window->format, color.R, color.G, color.B));
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	R = r;
	G = g;
	B = b;
	A = a;
}