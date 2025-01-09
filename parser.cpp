#include "restyle.h"

bool GetBinaryResource(LPCWSTR lpType, LPCWSTR lpName, LPVOID *ppvOut, DWORD *pcbOut)
{
	if (!lpType || !lpName || !ppvOut || !pcbOut)
		return false;

	*ppvOut = nullptr;
	*pcbOut = 0;

	HRSRC hrSrc = FindResourceW(g_hThemeModule, lpType, lpName);
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

bool ParseClassMap(void)
{
	LPVOID lpBase;
	DWORD dwSize;
	if (!GetBinaryResource(L"CMAP", L"CMAP", &lpBase, &dwSize))
		return false;

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
	LPBCMAP lpBCMap;
	DWORD dwSize;
	if (!GetBinaryResource(L"BCMAP", L"BCMAP", (LPVOID *)&lpBCMap, &dwSize))
		return false;

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

bool ParseVariantMap(void)
{
	LPVMAPENTRY lpVMap;
	DWORD dwSize;
	if (!GetBinaryResource(L"VMAP", L"VMAP", (LPVOID *)&lpVMap, &dwSize))
		return false;

	MSSTYLES_VARIANT var;
	LPVMAPENTRY lpEntry = lpVMap;
	for (int i = 0;; i++)
	{
		// if we exceed bounds
		if ((long long)lpEntry > ((long long)lpVMap + dwSize))
			break;

		// if string exceeds bounds
		if ((long long)lpEntry + sizeof(DWORD) + lpEntry->dwLength > ((long long)lpVMap + dwSize))
			break;

		LPWSTR pszBuffer = new WCHAR[lpEntry->dwLength];
		wcsncpy(pszBuffer, lpEntry->pszString, lpEntry->dwLength);
		switch (i % 3)
		{
			case 0:
				var.resourceName = pszBuffer;
				break;
			case 1:
				var.sizeName = pszBuffer;
				break;
			case 2:
				var.colorName = pszBuffer;
				g_variantMap.push_back(var);
				ZeroMemory(&var, sizeof(var));
				break;
		}
		delete[] pszBuffer;

		LPBYTE lpNewEntry = (LPBYTE)lpEntry;
		lpNewEntry += sizeof(DWORD);
		lpNewEntry += sizeof(WCHAR) * (lpEntry->dwLength + 1);
		lpEntry = (LPVMAPENTRY)lpNewEntry;
	}
}