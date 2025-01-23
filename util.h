#pragma once
#include "restyle.h"

int AsciiStrCmpI(const WCHAR *dst, const WCHAR *src);
bool GetBinaryResource(LPCWSTR lpType, LPCWSTR lpName, LPVOID *ppvOut, DWORD *pcbOut);

LPCWSTR GetPrimValueName(BYTE bPrimVal);
LPCWSTR GetSymbolValueName(long lSymbolVal);
LPCWSTR GetPartName(LPCWSTR pszClassName, int iPart);