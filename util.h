#pragma once
#include "restyle.h"

enum class ELogLevel
{
	Info = 0,
	Warning = 1,
	Fatal = 2,
};

// Outputs a user-displayed message to stderr. For output that can be written
// to files by the user (e.g. /precord option), use wprintf.
void Log(LPCWSTR pszFormat, ELogLevel eLevel, ...);
void Log(LPCWSTR pszFormat, ...);

int AsciiStrCmpI(const WCHAR *dst, const WCHAR *src);
bool GetBinaryResource(LPCWSTR lpType, LPCWSTR lpName, LPVOID *ppvOut, DWORD *pcbOut);

LPCWSTR GetSymbolValueName(long lSymbolVal);
EParseResult GetPropName(long lPartId, BYTE bPrimType, std::unique_ptr<WCHAR[]> &pszResult);
EParseResult GetPartAndStateName(
	LPCWSTR pszClassName,
	std::unique_ptr<WCHAR[]> &pszPartName,
	std::unique_ptr<WCHAR[]> &pszStateName,
	int iPart,
	int iState
);