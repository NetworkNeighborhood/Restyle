#pragma once
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>

extern WCHAR g_szThemeFilePath[MAX_PATH];
extern HMODULE g_hThemeModule;

typedef struct _BCMAP
{
	DWORD dwLength;
	DWORD pdwItems[1];
} BCMAP, *LPBCMAP;

typedef struct _BASECLASS
{
	DWORD dwBaseId;
	DWORD dwDerivedId;
} BASECLASS, *LPBASECLASS;

typedef struct _VMAPENTRY
{
	DWORD dwLength;
	WCHAR pszString[1];
} VMAPENTRY, *LPVMAPENTRY;

typedef struct _VS_VARIANT
{
	std::wstring resourceName;
	std::wstring sizeName;
	std::wstring colorName;
} VS_VARIANT;

extern std::vector<std::wstring> g_classMap;
extern std::vector<BASECLASS> g_baseClassMap;
extern std::vector<VS_VARIANT> g_variantMap;

#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

#define VER_MAJOR       0
#define VER_MINOR       1
#define VER_REVISION    0
#define VER_STRING      "0.1.0"

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)

#define __WDATE__  WIDE1(__DATE__)
#define __WTIME__  WIDE1(__TIME__)

__forceinline LPCWSTR NameOfClass(int id)
{
	#define NameOfClass(id) g_classMap.at(id).c_str()
}

bool ParseClassMap(void);
bool ParseBaseClassMap(void);
bool ParseVariantMap(void);