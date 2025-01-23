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
	const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
	const Restyle::TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable;
	for (int i = 0; i < pSchemaInfo->iPropCount; i++)
	{
		if (bPrimVal == pPropInfo[i].bPrimVal
		&& bPrimVal == pPropInfo[i].sEnumVal)
		{
			return pPropInfo[i].pszName;
		}
	}

	return L"Unknown primitive type";
}

LPCWSTR GetSymbolValueName(long lSymbolVal)
{
	const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
	const Restyle::TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable;
	for (int i = 0; i < pSchemaInfo->iPropCount; i++)
	{
		if (pPropInfo[i].bPrimVal == Restyle::TMT_ENUMDEF
		|| pPropInfo[i].bPrimVal == Restyle::TMT_ENUMVAL)
			continue;

		if (pPropInfo[i].sEnumVal == lSymbolVal)
			return pPropInfo[i].pszName;
	}

	return L"Unknown symbol";
}

LPCWSTR GetPartName(LPCWSTR pszClassName, int iPart)
{
	if (iPart == 0)
		return L"Common properties";

	const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
	const Restyle::TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable;

	LPCWSTR pszPartName = L"Unknown part";
	int iCompareLength = wcslen(pszClassName) + wcslen(L"PARTS") + 1;
	LPWSTR pszCompareName = new WCHAR[iCompareLength];
	swprintf(pszCompareName, iCompareLength, L"%sPARTS", pszClassName);
	bool fFoundPart = false;
	int iPartIndex = 0;

	for (int i = 0; i < pSchemaInfo->iPropCount; i++)
	{
		// First pass: find the class parts section
		if (!fFoundPart)
		{
			if (pPropInfo[i].bPrimVal != Restyle::TMT_ENUMDEF)
				continue;

			if (pPropInfo[i].pszName && 0 == AsciiStrCmpI(pPropInfo[i].pszName, pszCompareName))
			{
				fFoundPart = true;
				iPartIndex = i;
			}
		}
		// Second pass: find the part name
		else
		{
			if (pPropInfo[i].bPrimVal != Restyle::TMT_ENUMVAL)
			{
				// We've almost certainly moved on to a different enum/other definition
				// if this is the case.
				break;
			}

			if (pPropInfo[i].sEnumVal == iPart)
			{
				pszPartName = pPropInfo[i].pszName;
			}
		}
	}

	delete[] pszCompareName;
	return pszPartName;
}