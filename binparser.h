#pragma once
#include "restyle.h"

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

typedef struct _VSVARIANT
{
	std::wstring resourceName;
	std::wstring sizeName;
	std::wstring colorName;
} VSVARIANT;

typedef struct _VSRECORD
{
	long lSymbolVal;
	long lType;
	int iClass;
	int iPart;
	int iState;
	UINT uResID;
	long lReserved;
	int cbData;
} VSRECORD;

typedef bool (*RecordParserCallback)(const VSRECORD *lpRecord);

namespace BinParser
{
	std::vector<std::wstring> classMap;
	std::vector<BASECLASS> baseClassMap;
	std::vector<VSVARIANT> variantMap;

	bool ParseClassMap(void);
	bool ParseBaseClassMap(void);
	bool ParseVariantMap(void);
	bool ParseRecordResource(LPCWSTR lpType, LPCWSTR lpName, RecordParserCallback pfnCallback);

	inline LPCWSTR NameOfClass(UINT id)
	{
		if (classMap.size() < id)
			return classMap.at(id).c_str();
		return L"Invalid class name";
	}
};