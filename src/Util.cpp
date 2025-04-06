#include "Util.h"
#include "schema/SchemaUtils.h"

// #define NO_SCHEMAUTILS

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
#ifdef NO_SCHEMAUTILS
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
#else
	Restyle::SearchSchemaParams params = { 0 };
	params.cbSize = sizeof(params);
	params.eSearchQuery = Restyle::ESchemaSearchQuery::Property;
	params.sEnumVal = lSymbolVal;

	const Restyle::TMPROPINFO *pPropInfo = Restyle::SearchSchema(&params);
	
	if (pPropInfo)
	{
		return pPropInfo->pszName;
	}

	return nullptr;
#endif
}

LPCWSTR GetSymbolValueNameFromEnum(LPCWSTR pszEnumName, long lSymbolVal)
{
#ifdef NO_SCHEMAUTILS
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
#else
	const Restyle::TMPROPINFO *pPropInfo = Restyle::FindEnumValueInfo(pszEnumName, lSymbolVal);

	#if DEBUG
		if (pPropInfo && AsciiStrCmpI(pPropInfo->pszName, L"PenRightTap"))
		{
			Log(L"Huh?? %s %d \n", pszEnumName, lSymbolVal, ELogLevel::Info);
		}
	#endif

	return pPropInfo ? pPropInfo->pszName : nullptr;
#endif
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

// Gets the part and state name for a class name and part and state IDs.
// Getting state name is reliant on having the part name, so this is one
// function.
EParseResult GetPartAndStateName(
	LPCWSTR pszClassName,
	std::unique_ptr<WCHAR[]> &pszPartName,
	std::unique_ptr<WCHAR[]> &pszStateName,
	int iPart,
	int iState
)
{
	assert(pszClassName);

	int iEnumLength = wcslen(pszClassName) + sizeof("PARTS");
	std::unique_ptr<WCHAR[]> pszEnumName = std::make_unique<WCHAR[]>(iEnumLength);
	swprintf_s(pszEnumName.get(), iEnumLength, L"%sPARTS", pszClassName);
	LPCWSTR pszTempPartName = GetSymbolValueNameFromEnum(pszEnumName.get(), iPart);
	if (pszTempPartName)
	{
		int iPartLength = wcslen(pszTempPartName) + 1;
		pszPartName = std::make_unique<WCHAR[]>(iPartLength);
		wcscpy_s(pszPartName.get(), iPartLength, pszTempPartName);

		iEnumLength = wcslen(pszTempPartName) + sizeof("STATES");
		pszEnumName = std::make_unique<WCHAR[]>(iEnumLength);
		swprintf_s(pszEnumName.get(), iEnumLength, L"%sSTATES", pszTempPartName);
		LPCWSTR pszTempStateName = GetSymbolValueNameFromEnum(pszEnumName.get(), iState);
		if (pszTempStateName)
		{
			int iStateLength = wcslen(pszTempStateName) + 1;
			pszStateName = std::make_unique<WCHAR[]>(iStateLength);
			wcscpy_s(pszStateName.get(), iStateLength, pszTempStateName);
			return EParseResult::Success;
		}
	}

	WCHAR szStateId[INT_STRING_CCH_MAX];
	swprintf_s(szStateId, L"%d", iState);
	int iStateLength = sizeof("*State") + wcslen(szStateId);
	pszStateName = std::make_unique<WCHAR[]>(iStateLength);
	swprintf_s(pszStateName.get(), iStateLength, L"*State%d", iState);
	Log(
		L"WARNING: Unrecognized state %d for class '%s' part %d, using fallback name '%s'\n",
		ELogLevel::Warning, iState, pszClassName, iPart, pszStateName.get()
	);

	if (!pszTempPartName)
	{
		WCHAR szPartId[INT_STRING_CCH_MAX];
		swprintf_s(szPartId, L"%d", iPart);
		int iPartLength = sizeof("*Part") + wcslen(szPartId);
		pszPartName = std::make_unique<WCHAR[]>(iPartLength);
		swprintf_s(pszPartName.get(), iPartLength, L"*Part%d", iPart);
		Log(
			L"WARNING: Unrecognized part %d for class '%s', using fallback name '%s'\n",
			ELogLevel::Warning, iPart, pszClassName, pszPartName.get()
		);
	}

	return EParseResult::UnknownType;
}

bool StrEndsWithW(LPCWSTR a, LPCWSTR b)
{
	if (wcslen(a) >= wcslen(b))
	{
		if (wcscmp(a + wcslen(a) - wcslen(b), b) == 0)
		{
			return true;
		}
	}

	return false;
}