#include "util.h"

void LogV(LPCWSTR pszFormat, ELogLevel eLevel, va_list args)
{
	// colorize
	switch (eLevel)
	{
		case ELogLevel::Warning:
			fwprintf(stderr, L"\x1b[1;33m");
			break;
		case ELogLevel::Fatal:
			fwprintf(stderr, L"\x1b[1;31m");
			break;
	}

	vfwprintf(stderr, pszFormat, args);

	// reset color
	fwprintf(stderr, L"\x1b[0m");
}

void Log(LPCWSTR pszFormat, ELogLevel eLevel, ...)
{
	va_list args;
	va_start(args, eLevel);
	LogV(pszFormat, eLevel, args);
}

void Log(LPCWSTR pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	LogV(pszFormat, ELogLevel::Info, args);
}

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
		Log(L"FATAL: Failed to find %s resource.\n", ELogLevel::Fatal, lpType);
		return false;
	}

	HGLOBAL hGlobal = LoadResource(g_hThemeModule, hrSrc);
	if (!hGlobal)
	{
		Log(L"FATAL: Failed to load %s resource.\n", ELogLevel::Fatal, lpType);
		return false;
	}

	DWORD dwSize = SizeofResource(g_hThemeModule, hrSrc);
	LPVOID lpBase = LockResource(hGlobal);
	if (!dwSize || !lpBase)
	{
		Log(L"FATAL: Failed to get address and/or size of %s resouce.\n", ELogLevel::Fatal, lpType);
		return false;
	}

	*ppvOut = lpBase;
	*pcbOut = dwSize;
	return true;
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
	return nullptr;
}

LPCWSTR GetSymbolValueNameFromEnum(LPCWSTR pszEnumName, long lSymbolVal)
{
	const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
	const Restyle::TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable;
	bool fFoundEnum = false;
	int iEnumIndex = 0;
	for (int i = 0; i < pSchemaInfo->iPropCount; i++)
	{
		// First pass: find the class parts section
		if (!fFoundEnum)
		{
			if (pPropInfo[i].bPrimVal != Restyle::TMT_ENUMDEF)
				continue;

			if (pPropInfo[i].pszName && 0 == AsciiStrCmpI(pPropInfo[i].pszName, pszEnumName))
			{
				fFoundEnum = true;
				iEnumIndex = i;
			}
		}
		// Second pass: find the part name
		else
		{
			if (pPropInfo[i].bPrimVal != Restyle::TMT_ENUMVAL)
				// We've almost certainly moved on to a different enum/other definition
				// if this is the case.
				break;

			if (pPropInfo[i].sEnumVal == lSymbolVal)
				return pPropInfo[i].pszName;
		}
	}
	return nullptr;
}

EParseResult GetPropName(long lPartId, BYTE bPrimType, std::unique_ptr<WCHAR[]> &pszResult)
{
	LPCWSTR pszPropName = GetSymbolValueName(lPartId);
	if (pszPropName)
	{
		int length = wcslen(pszPropName) + 1;
		pszResult = std::make_unique<WCHAR[]>(length);
		wcscpy_s(pszResult.get(), length, pszPropName);
		return EParseResult::Success;
	}

	LPCWSTR pszTypeName = GetSymbolValueName(bPrimType);
	if (!pszTypeName)
	{
		Log(L"FATAL: Unrecognized primitive type %d\n", ELogLevel::Fatal, bPrimType);
		return EParseResult::Fail;
	}

	WCHAR szPartId[INT_STRING_CCH_MAX];
	swprintf_s(szPartId, L"%d", lPartId);

	int length = sizeof("*Prop<>") + wcslen(szPartId) + wcslen(pszTypeName);
	pszResult = std::make_unique<WCHAR[]>(length);
	swprintf_s(pszResult.get(), length, L"*Prop%d<%s>", lPartId, pszTypeName);
	Log(L"WARNING: Unrecognized property %d, using fallback name '%s'\n", ELogLevel::Warning, lPartId, pszResult.get());
	return EParseResult::UnknownType;
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