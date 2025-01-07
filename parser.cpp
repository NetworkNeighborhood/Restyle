#include "restyle.h"

bool ParseClassMap(void)
{
	HRSRC hrSrc = FindResourceW(g_hThemeModule, L"CMAP", L"CMAP");
	if (!hrSrc)
	{
		fwprintf(stderr, L"Fatal: Failed to find CMAP.\n");
		return false;
	}

	HGLOBAL hGlobal = LoadResource(g_hThemeModule, hrSrc);
	if (!hGlobal)
	{
		fwprintf(stderr, L"Fatal: Failed to load CMAP.\n");
		return false;
	}

	DWORD dwSize = SizeofResource(g_hThemeModule, hrSrc);
	LPVOID lpBase = LockResource(hGlobal);
	if (!dwSize || !lpBase)
	{
		fwprintf(stderr, L"Fatal: Failed to get address and/or size of CMAP.\n");
		return false;
	}

	LPWSTR pszClass = (LPWSTR)lpBase;
	while ((UINT_PTR)pszClass < ((UINT_PTR)lpBase + dwSize))
	{
		if (*pszClass)
			g_classMap.push_back(pszClass);
		pszClass += wcslen(pszClass) + 1;
	}

	return true;
}

bool ParseBaseClassMap(void)
{
	HRSRC hrSrc = FindResourceW(g_hThemeModule, L"BCMAP", L"BCMAP");
	if (!hrSrc)
	{
		fwprintf(stderr, L"Fatal: Failed to find BCMAP.\n");
		return false;
	}

	HGLOBAL hGlobal = LoadResource(g_hThemeModule, hrSrc);
	if (!hGlobal)
	{
		fwprintf(stderr, L"Fatal: Failed to load BCMAP.\n");
		return false;
	}

	DWORD dwSize = SizeofResource(g_hThemeModule, hrSrc);
	LPBCMAP lpBCMap = (LPBCMAP)LockResource(hGlobal);
	if (!dwSize || !lpBCMap)
	{
		fwprintf(stderr, L"Fatal: Failed to get address and/or size of BCMAP.\n");
		return false;
	}

	for (DWORD i = 0; i < lpBCMap->dwLength; i++)
	{
		// 0xFFFFFFFF means no inherited class
		if (lpBCMap->pdwItems[i] != UINT32_MAX)
		{
			BASECLASS bc = { lpBCMap->pdwItems[i], i };
			g_baseClassMap.push_back(bc);
		}
	}

	return true;
}