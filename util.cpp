#include "util.h"

#define __ascii_towlower(c)  ( (((c) >= L'A') && ((c) <= L'Z')) ? ((c) - L'A' + L'a') : (c) )
int AsciiStrCmpI(const WCHAR *dst, const WCHAR *src)
{
	WCHAR a, b;

	if (!dst)
	{
		return src ? -1 : 0;
	}
	else if (!src)
	{
		return 1;
	}

	do
	{
		a = __ascii_towlower(*dst);
		b = __ascii_towlower(*src);
		dst++;
		src++;
	}
	while (a && (a == b));

	return (int)(a - b);
}

bool GetBinaryResource(LPCWSTR lpType, LPCWSTR lpName, LPVOID *ppvOut, DWORD *pcbOut)
{
	if (!lpType || !lpName || !ppvOut || !pcbOut)
		return false;

	*ppvOut = nullptr;
	*pcbOut = 0;

	HRSRC hrSrc = FindResourceExW(g_hThemeModule, lpType, lpName, 0);
	if (!hrSrc)
	{
		fwprintf(stderr, L"Fatal: Failed to find %s resource.\n", lpType);
		return false;
	}

	HGLOBAL hGlobal = LoadResource(g_hThemeModule, hrSrc);
	if (!hGlobal)
	{
		fwprintf(stderr, L"Fatal: Failed to load %s resource.\n", lpType);
		return false;
	}

	DWORD dwSize = SizeofResource(g_hThemeModule, hrSrc);
	LPVOID lpBase = LockResource(hGlobal);
	if (!dwSize || !lpBase)
	{
		fwprintf(stderr, L"Fatal: Failed to get address and/or size of %s resouce.\n", lpType);
		return false;
	}

	*ppvOut = lpBase;
	*pcbOut = dwSize;
	return true;
}

LPCWSTR GetPrimValueName(BYTE bPrimVal)
{
	switch (bPrimVal)
	{
		case Restyle::TMT_ENUM: return L"Enum";
			// case Restyle::TMT_ENUMDEF: return L"EnumDef";
			// case Restyle::TMT_ENUMVAL: return L"EnumVal";
		case Restyle::TMT_STRING: return L"String";
		case Restyle::TMT_INT: return L"Int";
		case Restyle::TMT_BOOL: return L"Bool";
		case Restyle::TMT_COLOR: return L"Color";
		case Restyle::TMT_MARGINS: return L"Margins";
		case Restyle::TMT_FILENAME: return L"FileName";
		case Restyle::TMT_SIZE: return L"Size";
		case Restyle::TMT_POSITION: return L"Position";
		case Restyle::TMT_RECT: return L"Rect";
		case Restyle::TMT_FONT: return L"Font";
		case Restyle::TMT_INTLIST: return L"IntList";
	}

	return L"Unknown primitive type";
}