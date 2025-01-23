#include "restyle.h"
#include <uxtheme.h>
#include "schemapriv.h"
#include "util.h"

#include "restyle_TmSchema.h"
#define SCHEMA_STRINGS
#include "restyle_TmSchema.h"

#include "SchemaPriv.h"

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

bool ParseClassMap(void)
{
	LPVOID lpResource;
	DWORD dwSize;
	if (!GetBinaryResource(L"CMAP", L"CMAP", &lpResource, &dwSize))
		return false;

	LPWSTR pszClass = (LPWSTR)lpResource;
	while ((UINT_PTR)pszClass < ((UINT_PTR)lpResource + dwSize))
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

	VSVARIANT var;
	LPVMAPENTRY lpEntry = lpVMap;
	for (int i = 0;; i++)
	{
		// if we exceed bounds
		if ((long long)lpEntry > ((long long)lpVMap + dwSize))
			break;

		// if string exceeds bounds
		if ((long long)lpEntry + sizeof(DWORD) + (sizeof(WCHAR) * lpEntry->dwLength) > ((long long)lpVMap + dwSize))
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
	
	return true;
}

static LPCWSTR GetPrimValueName(BYTE bPrimVal)
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

bool ParseRecordResource(LPCWSTR lpType, LPCWSTR lpName)
{
	LPVOID lpResource;
	DWORD dwSize;
	if (!GetBinaryResource(lpType, lpName, &lpResource, &dwSize))
		return false;
	
	// We wanna parse the class map so we can see symbol values too:
	ParseClassMap();
	
	const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
	const Restyle::TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable;
	
	VSRECORD *lpRecord = (VSRECORD *)lpResource;
	while ((long long)lpRecord < ((long long)lpResource + dwSize))
	{
		LPCWSTR szSymbolVal = L"Invalid symbol";
		
		for (int i = 0; i < pSchemaInfo->iPropCount; i++)
		{
			if (pPropInfo[i].bPrimVal == Restyle::TMT_ENUMDEF)
				continue;
			
			if (pPropInfo[i].bPrimVal == Restyle::TMT_ENUMVAL)
				continue;
			
			if (pPropInfo[i].sEnumVal == lpRecord->lSymbolVal)
			{
				szSymbolVal = pPropInfo[i].pszName;
			}
		}
		
		LPCWSTR szClassName = L"Invalid class name pointer";
		
		if (lpRecord->iClass > 0 && lpRecord->iClass < g_classMap.size())
		{
			szClassName = g_classMap[lpRecord->iClass].c_str();
		}
		
		LPCWSTR szType = GetPrimValueName(lpRecord->lType);
		
		LPCWSTR szPartName = L"(N/A)";
		if (lpRecord->iPart == 0)
		{
			szPartName = L"Common properties";
		}
		else if (szClassName)
		{
			szPartName = L"Invalid part name";
			LPWSTR szCompareName = new WCHAR[wcslen(szClassName) + wcslen(L"PARTS") + 1];
			swprintf(szCompareName, wcslen(szClassName) + wcslen(L"PARTS") + 1, L"%sPARTS", szClassName);
			bool fFoundPart = false;
			int iPartIndex = 0;
			
			for (int i = 0; i < pSchemaInfo->iPropCount; i++)
			{
				// First pass: find the class parts section
				if (!fFoundPart)
				{
					if (pPropInfo[i].bPrimVal != Restyle::TMT_ENUMDEF)
					{
						continue;
					}
					
					if (pPropInfo[i].pszName && AsciiStrCmpI(pPropInfo[i].pszName, szCompareName) == 0)
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
					
					if (pPropInfo[i].sEnumVal == lpRecord->iPart)
					{
						szPartName = pPropInfo[i].pszName;
					}
				}
			}
			
			delete[] szCompareName;
		}
		
		wprintf(
			L"====================\n"
			L"lSymbolVal: %d (%s)\n"
			L"lType: %d (%s)\n"
			L"iClass: %d (%s)\n"
			L"iPart: %d (%s)\n"
			L"iState: %d\n"
			L"uResID: %u\n"
			L"lReserved: %d\n"
			L"cbData: %d\n",
			lpRecord->lSymbolVal, szSymbolVal, 
			lpRecord->lType, szType,
			lpRecord->iClass, szClassName,
			lpRecord->iPart, szPartName,
			lpRecord->iState,
			lpRecord->uResID,
			lpRecord->lReserved,
			lpRecord->cbData
		);
		
		// Parse value:
		if (lpRecord->cbData != 0)
		{
			switch (lpRecord->lType)
			{
				case Restyle::TMT_BOOL:
				{
					if (lpRecord->cbData < sizeof(BOOL))
					{
						break;
					}
					
					BOOL fValue = *(BOOL *)((BYTE *)lpRecord + sizeof(VSRECORD));
					wprintf(L"Value = %s\n", fValue ? L"true" : L"false");
					break;
				}
				
				case Restyle::TMT_INT:
				case Restyle::TMT_SIZE:
				{
					if (lpRecord->cbData < sizeof(int))
					{
						break;
					}
					
					int iValue = *(int *)((BYTE *)lpRecord + sizeof(VSRECORD));
					wprintf(L"Value = %d\n", iValue);
					break;
				}
				
				case Restyle::TMT_ENUM:
				{
					if (lpRecord->cbData < sizeof(int))
					{
						break;
					}
					
					int eValue = *(int *)((BYTE *)lpRecord + sizeof(VSRECORD));
					LPCWSTR szEnumName = nullptr;
					int iEnumDefOffset = 0;
					LPCWSTR szValueName = nullptr;
					
					// First pass: find the enum name in the property table:
					for (int i = 0; i < pSchemaInfo->iPropCount; i++)
					{
						if (pPropInfo[i].bPrimVal == Restyle::TMT_ENUM && pPropInfo[i].sEnumVal == lpRecord->lSymbolVal)
						{
							szEnumName = pPropInfo[i].pszName;
							break;
						}
					}
					// Second pass: find the enum definition in the schema:
					for (int i = 0; i < pSchemaInfo->iPropCount; i++)
					{
						if (pPropInfo[i].bPrimVal == Restyle::TMT_ENUMDEF && AsciiStrCmpI(szEnumName, pPropInfo[i].pszName) == 0)
						{
							iEnumDefOffset = i + 1;
							break;
						}
					}
					// Third pass: find the enum value name:
					for (int i = iEnumDefOffset; i < pSchemaInfo->iPropCount; i++)
					{
						if (pPropInfo[i].bPrimVal != Restyle::TMT_ENUMVAL)
						{
							break;
						}
						
						if (pPropInfo[i].sEnumVal == eValue)
						{
							szValueName = pPropInfo[i].pszName;
							break;
						}
					}
					
					if (szValueName)
					{
						wprintf(L"Value = %s (%d)\n", szValueName, eValue);
					}
					else
					{
						wprintf(L"Value = %d\n", eValue);
					}
					
					break;
				}
				
				case Restyle::TMT_STRING:
				{
					LPCWSTR szValue = (LPCWSTR)((BYTE *)lpRecord + sizeof(VSRECORD));
					wprintf(L"Value = \"%s\"\n", szValue);
					break;
				}
				
				case Restyle::TMT_RECT:
				{
					if (lpRecord->cbData > sizeof(RECT))
					{
						break;
					}
					
					RECT *lpRect = (RECT *)((BYTE *)lpRecord + sizeof(VSRECORD));
					wprintf(L"Value = (LTRB){ %d, %d, %d, %d }\n", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
					break;
				}
				
				case Restyle::TMT_MARGINS:
				{
					if (lpRecord->cbData > sizeof(MARGINS))
					{
						break;
					}
					
					MARGINS *lpMargins = (MARGINS *)((BYTE *)lpRecord + sizeof(VSRECORD));
					wprintf(L"Value = (LRTB){ %d, %d, %d, %d }\n", lpMargins->cxLeftWidth, lpMargins->cxRightWidth, lpMargins->cyTopHeight, lpMargins->cyBottomHeight);
					break;
				}
				
				case Restyle::TMT_INTLIST:
				{
					if (lpRecord->cbData > sizeof(int))
					{
						break;
					}
					
					INTLIST *lpIntList = (INTLIST *)((BYTE *)lpRecord + sizeof(VSRECORD));
					
					// Revalidate:
					if (lpRecord->cbData > sizeof(int) * lpIntList->iValueCount)
					{
						break;
					}
					
					wprintf(L"Value = { ");
					
					for (int i = 0; i < lpIntList->iValueCount; i++)
					{
						wprintf(L"%d", lpIntList->iValues[i]);
						
						if (i != lpIntList->iValueCount - 1)
						{
							wprintf(L", ");
						}
					}
					
					wprintf(L" }\n");
					break;
				}
				
				case Restyle::TMT_POSITION:
				{
					if (lpRecord->cbData > sizeof(POINT))
					{
						break;
					}
					
					POINT *lpPoint = (POINT *)((BYTE *)lpRecord + sizeof(VSRECORD));
					wprintf(L"Value = (XY){ %d, %d }\n", lpPoint->x, lpPoint->y);
					break;
				}
				
				case Restyle::TMT_COLOR:
				{
					if (lpRecord->cbData > sizeof(COLORREF))
					{
						break;
					}
					
					COLORREF crColor = *(COLORREF *)((BYTE *)lpRecord + sizeof(VSRECORD));
					BYTE bRed = GetRValue(crColor);
					BYTE bGreen = GetGValue(crColor);
					BYTE bBlue = GetBValue(crColor);
					BYTE bExtra = crColor >> 24;
					
					wprintf(L"Value = (RGBA){ %d, %d, %d, %d }\n", bRed, bGreen, bBlue, bExtra);
					break;
				}
			}
		}

		DWORD dwNextOffset = 0;
		if (!lpRecord->uResID)
			dwNextOffset = lpRecord->cbData;
		// ????????
		// this is what uxtheme does soooooooo
		dwNextOffset = (dwNextOffset + (sizeof(VSRECORD) | 7)) & 0xFFFFFFF8;

		lpRecord = (VSRECORD *)((char *)lpRecord + dwNextOffset);
	}
	
	return true;
}