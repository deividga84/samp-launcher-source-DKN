#include "../main.h"
#include "font.h"

void CFont::AsciiToGxtChar(const char* ascii, uint16_t* gxt)
{
	return ((void(*)(const char*, uint16_t*))(g_libGTASA + /*0x532D00*/0x5A8350 + 1))(ascii, gxt);
}

void CFont::SetScale(float x, float y)
{
	*(float*)(g_libGTASA + 0xA297B8) = x;
	*(float*)(g_libGTASA + 0xA297BC) = y;
}

void CFont::SetColor(uint32_t *dwColor)
{
	return ((void(*)(uint32_t*))(g_libGTASA + 0x5AAFC8 + 1))(dwColor);
}

void CFont::SetJustify(uint8_t justify)
{
	return ((void(*)(uint8_t))(g_libGTASA + 0x5AB2F4 + 1))(justify);
}

void CFont::SetOrientation(uint8_t orientation)
{
	return ((void(*)(uint8_t))(g_libGTASA + 0x5AB304 + 1))(orientation);
}

void CFont::SetWrapX(float wrapX)
{
	return ((void(*)(float))(g_libGTASA + 0x5AB1D8 + 1))(wrapX);
}

void CFont::SetCentreSize(float size)
{
	return ((void(*)(float))(g_libGTASA + 0x5AB1E8 + 1))(size);
}

void CFont::SetBackground(uint8_t bBackground, uint8_t bOnlyText)
{
	return ((void(*)(uint8_t, uint8_t))(g_libGTASA + 0x5AB2C0 + 1))(bBackground, bOnlyText);
}

void CFont::SetBackgroundColor(uint32_t *dwColor)
{
	return ((void(*)(uint32_t*))(g_libGTASA + 0x5AB2D0 + 1))(dwColor);
}

void CFont::SetProportional(uint8_t prop)
{
	return ((void(*)(uint8_t))(g_libGTASA + 0x5AB2B0 + 1))(prop);
}

void CFont::SetDropColor(uint32_t* dwColor)
{
	return ((void(*)(uint32_t*))(g_libGTASA + 0x5AB218 + 1))(dwColor);
}

void CFont::SetDropShadowPosition(uint8_t pos)
{
	return ((void(*)(uint8_t))(g_libGTASA + 0x5A8A6C + 1))(pos);
}

void CFont::PrintString(float fX, float fY, const uint16_t* szText)
{
	return ((void(*)(float, float, const uint16_t*))(g_libGTASA + 0x5AA190 + 1))(fX, fY, szText);
}

/*void CFont::PrintString(float posX, float posY, const char* string)
{
	uint16_t* gxt_string = new uint16_t[0xFF];
	CFont::AsciiToGxtChar(string, gxt_string);
	((void (*)(float, float, uint16_t*))(g_libGTASA + 0x5AA190 + 1))(posX, posY, gxt_string);
	delete gxt_string;
	((void (*)())(g_libGTASA + 0x5A90B0 + 1))();//53411C ; _DWORD CFont::RenderFontBuffer(CFont *__hidden this)
}*/

void CFont::SetFontStyle(uint8_t style)
{
	return ((void(*)(uint8_t))(g_libGTASA + 0x5AB14C + 1))(style);
}

void CFont::SetEdge(uint8_t edge)
{
	return ((void(*)(uint8_t))(g_libGTASA + 0x5AB27C + 1))(edge);
}