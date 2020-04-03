#include "graphics.h"
int Graphics::posX = 0;
int Graphics::posY = 0;
int Graphics::width = 0;
int Graphics::height = 0;
int Graphics::thickness = 0;
int Graphics::color = 0;
map<string, GraphicalTexture> Graphics::textures;
map<string, FontType> Graphics::fonts;

u8g2_t Graphics::u8g2;

void Graphics::InitGraphicSystem()
{
	u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_MIRROR, u8x8_byte_arm_linux_hw_spi, u8x8_arm_linux_gpio_and_delay);
	u8x8_SetPin(u8g2_GetU8x8(&u8g2), U8X8_PIN_DC, 5);
	u8x8_SetPin(u8g2_GetU8x8(&u8g2), U8X8_PIN_RESET, 6);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_SetFontMode(&u8g2, 1);
	ClearScreen();
}
void Graphics::ClearScreen()
{
	u8g2_ClearBuffer(&u8g2);
}
void Graphics::DrawPixel()
{
	u8g2_DrawPixel(&u8g2, posX, posY);
}
void Graphics::DrawRectangle()
{
	u8g2_DrawFrame(&u8g2, posX, posY, width, height);
}
void Graphics::DrawFilledRectangle(int fill)
{
	u8g2_DrawFrame(&u8g2, posX, posY, width, height);
	u8g2_SetDrawColor(&u8g2, fill);
	u8g2_DrawBox(&u8g2, posX + 1, posY + 1, width - 2, height - 2);
	u8g2_SetDrawColor(&u8g2, color);
}
int Graphics::GetTextWidth(string text, FontType font)
{
	u8g2_SetFont(&u8g2, (const uint8_t*)font.data);
	return u8g2_GetStrWidth(&u8g2, text.c_str());
}
void Graphics::DrawImage(string id)
{
	u8g2_DrawXBM(&u8g2, posX, posY, width, height, textures[id].data);
}
void Graphics::DrawString(string text, FontType font)
{
	u8g2_SetFont(&u8g2, font.data);
	u8g2_DrawStr(&u8g2, posX, posY, text.c_str());
}
void Graphics::LoadImage(string path, string id)
{

}
void Graphics::LoadFont(string path, string id)
{

}
void Graphics::UpdateScreen()
{
	u8g2_SendBuffer(&u8g2);
}
void Graphics::SetPos(int x, int y)
{
	posX = x;
	posY = y;
}
void Graphics::SetSize(int w, int h)
{
	width = w;
	height = h;
}
void Graphics::SetThickness(int thick)
{
	thickness = thick;
}
void Graphics::SetColor(int c)
{
	color = c;
	u8g2_SetDrawColor(&u8g2, c);
}