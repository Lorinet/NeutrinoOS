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
bool Graphics::running = false;
void Graphics::InitGraphicSystem()
{
	if (!running)
	{
#if defined(__UNIX)
		u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_MIRROR, u8x8_byte_arm_linux_hw_spi, u8x8_arm_linux_gpio_and_delay);
		u8x8_SetPin(u8g2_GetU8x8(&u8g2), U8X8_PIN_DC, 5);
		u8x8_SetPin(u8g2_GetU8x8(&u8g2), U8X8_PIN_RESET, 6);
#elif defined(__ESP32)
		u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
		u8g2_esp32_hal.sda = GPIO_NUM_21;
		u8g2_esp32_hal.scl = GPIO_NUM_22;
		u8g2_esp32_hal_init(u8g2_esp32_hal);
		u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8g2_esp32_i2c_byte_cb, u8g2_esp32_gpio_and_delay_cb);
#endif
		u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);
		u8g2_InitDisplay(&u8g2);
		u8g2_SetPowerSave(&u8g2, 0);
		u8g2_ClearBuffer(&u8g2);
		u8g2_SetFontMode(&u8g2, 1);
		u8g2_SetFont(&u8g2, u8g2_font_u8glib_4_tf);
		u8g2_DrawStr(&u8g2, 2, 5, "Neutrino Core OS [Version 1.0f]");
		u8g2_SendBuffer(&u8g2);
		running = true;
	}
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
	u8g2_SetFont(&u8g2, (const uint8_t *)font.data);
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
void Graphics::DrawStringWordWrap(string text, FontType font)
{
	u8g2_SetFont(&u8g2, font.data);
	char ps[2];
	ps[1] = 0;
	int opx = posX;
	int opy = posY;
	for (int i = 0; i < text.size(); i++)
	{
		ps[0] = text[i];
		if (ps[0] != '\n')
		{
			u8g2_DrawStr(&u8g2, posX, posY + font.height, ps);
			posX += u8g2_GetStrWidth(&u8g2, ps) + 1;
		}
		else
		{
			posY += font.height + 1;
			posX = opx;
		}
		if (posX >= resX || posX >= width + opx)
		{
			posX = opx;
			posY += font.height + 1;
		}
		if (posY >= height + opy)
		{
			SetColor(0);
			u8g2_DrawBox(&u8g2, opx, opy, width, height);
			posX = opx;
			posY = opy;
			SetColor(1);
		}
	}
}
void Graphics::DrawStringWordWrapEx(string text, FontType font, int& curx, int& cury)
{
	u8g2_SetFont(&u8g2, font.data);
	char ps[2];
	ps[1] = 0;
	for (int i = 0; i < text.size(); i++)
	{
		ps[0] = text[i];
		if (ps[0] != '\n')
		{
			u8g2_DrawStr(&u8g2, posX, posY + font.height, ps);
			posX += u8g2_GetStrWidth(&u8g2, ps) + 1;
		}
		else
		{
			posY += font.height + 1;
			posX = 0;
		}
		if (posX >= resX)
		{
			posX = 0;
			posY += font.height + 1;
		}
		if (posY >= resY)
		{
			SetColor(0);
			u8g2_DrawBox(&u8g2, 0, 0, resX, resY);
			posX = 0;
			posY = 0;
			SetColor(1);
		}
	}
	curx = posX;
	cury = posY;
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