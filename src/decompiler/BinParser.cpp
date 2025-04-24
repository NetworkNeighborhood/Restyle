#include "Restyle.h"
#include "BinParser.h"
#include "schema/SchemaPriv.h"
#include "schema/SchemaUtils.h"
#include "Util.h"

namespace BinParser
{

std::vector<std::wstring> classMap;
std::vector<BASECLASS> baseClassMap;
std::vector<VSVARIANT> variantMap;

bool ParseClassMap(void)
{
	Log(L"Parsing class map...\n");

	LPVOID lpResource;
	DWORD dwSize;
	if (!GetBinaryResource(L"CMAP", L"CMAP", &lpResource, &dwSize))
		return false;

	LPWSTR pszClass = (LPWSTR)lpResource;
	while ((UINT_PTR)pszClass < ((UINT_PTR)lpResource + dwSize))
	{
		if (*pszClass)
			classMap.push_back(pszClass);
		pszClass += wcslen(pszClass) + 1;
	}

	return true;
}

bool ParseBaseClassMap(void)
{
	Log(L"Parsing base class map...\n");

	LPBCMAP lpBCMap;
	DWORD dwSize;
	if (!GetBinaryResource(L"BCMAP", L"BCMAP", (LPVOID *)&lpBCMap, &dwSize))
		return false;

	DWORD dwGlobalsId = IDOfClass(L"globals");
	if (dwGlobalsId == (DWORD)-1)
	{
		Log(L"FATAL: Missing 'globals' class. Cannot parse base class map.\n", ELogLevel::Fatal);
		return false;
	}

	for (DWORD i = 0; i < lpBCMap->dwLength; i++)
	{
		// 0xFFFFFFFF means no inherited class
		if (lpBCMap->pdwItems[i] != (DWORD)-1)
		{
			BASECLASS bc = { 
				dwGlobalsId + lpBCMap->pdwItems[i],
				dwGlobalsId + i
			};
			baseClassMap.push_back(bc);
		}
	}

	return true;
}

bool ParseVariantMap(void)
{
	Log(L"Parsing variant map...\n");

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
		{
			Log(L"WARNING: Variant name length exceeds bounds of variant map, stopping enumeration.\n", ELogLevel::Warning);
			break;
		}

		LPWSTR pszBuffer = new WCHAR[lpEntry->dwLength + 1];
		wcsncpy(pszBuffer, lpEntry->pszString, lpEntry->dwLength);
		pszBuffer[lpEntry->dwLength] = L'\0';
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
				variantMap.push_back(var);
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

bool ParseRecordResource(LPCWSTR lpType, LPCWSTR lpName, void *lpParam, RecordParserCallback pfnCallback)
{
	assert(pfnCallback);

	LPVOID lpResource;
	DWORD dwSize;
	if (!GetBinaryResource(lpType, lpName, &lpResource, &dwSize))
		return false;
	
	VSRECORD *lpRecord = (VSRECORD *)lpResource;
	while ((long long)lpRecord < ((long long)lpResource + dwSize))
	{
		if (!pfnCallback(lpRecord, lpParam))
			break;

		DWORD dwNextOffset = 0;
		if (!lpRecord->uResID)
			dwNextOffset = lpRecord->cbData;
		// Aligns on 8-bit boundary. UXTheme does this.
		dwNextOffset = (dwNextOffset + (sizeof(VSRECORD) | 7)) & 0xFFFFFFF8;
		lpRecord = (VSRECORD *)((char *)lpRecord + dwNextOffset);
	}
	
	return true;
}

LPCWSTR NameOfClass(UINT id)
{
	if (id < classMap.size())
		return classMap.at(id).c_str();
	return nullptr;
}

DWORD IDOfClass(LPCWSTR pszClassName)
{
	assert(pszClassName);
	size_t length = classMap.size();
	for (size_t i = 0; i < length; i++)
	{
		if (0 == _wcsicmp(classMap.at(i).c_str(), pszClassName))
			return i;
	}
	return (DWORD)-1;
}


DWORD GetBaseClass(UINT id)
{
	for (const BASECLASS &bc : baseClassMap)
	{
		if (bc.dwDerivedId == id)
			return bc.dwBaseId;
	}
	return (DWORD)-1;
}

EParseResult GetRecordValueString(const VSRECORD *lpRecord, LPWSTR pszBuffer, DWORD cchBufferMax)
{
	assert(lpRecord && pszBuffer && cchBufferMax);

	ZeroMemory(pszBuffer, cchBufferMax * sizeof(WCHAR));

	switch (lpRecord->lType)
	{
		case Restyle::TMT_BOOL:
		{
			if (lpRecord->cbData < sizeof(BOOL))
				return EParseResult::Fail;

			BOOL fValue = *(BOOL *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"%s", fValue ? L"true" : L"false");
			return EParseResult::Success;
		}

		case Restyle::TMT_INT:
		case Restyle::TMT_SIZE:
		{
			if (lpRecord->cbData < sizeof(int))
				return EParseResult::Fail;

			int iValue = *(int *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"%d", iValue);
			return EParseResult::Success;
		}

		case Restyle::TMT_FLOAT:
		{
			if (lpRecord->cbData < sizeof(float))
				return EParseResult::Fail;

			float flValue = *(float *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"%f", flValue);
			return EParseResult::Success;
		}

		case Restyle::TMT_ENUM:
		{
			if (lpRecord->cbData < sizeof(int))
				return EParseResult::Fail;

			const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
			const Restyle::TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable;

			int eValue = *(int *)((BYTE *)lpRecord + sizeof(VSRECORD));
			LPCWSTR szEnumName = nullptr;
			LPCWSTR szValueName = nullptr;

//#define NO_SCHEMAUTILS
#ifdef NO_SCHEMAUTILS
			int iEnumDefOffset = 0;

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
				if (pPropInfo[i].bPrimVal == Restyle::TMT_ENUMDEF && 0 == AsciiStrCmpI(szEnumName, pPropInfo[i].pszName))
				{
					iEnumDefOffset = i + 1;
					break;
				}
			}
			// Third pass: find the enum value name:
			for (int i = iEnumDefOffset; i < pSchemaInfo->iPropCount; i++)
			{
				if (Restyle::IsMetadataType(pPropInfo[i].bPrimVal)) continue;

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
#else
			const Restyle::TMPROPINFO *pEnumValInfo = Restyle::FindEnumValueInfo(lpRecord->lSymbolVal, eValue);
			const Restyle::TMPROPINFO *pEnumDefInfo = Restyle::FindEnumDefForEnumVal(pEnumValInfo);

			if (pEnumDefInfo)
				szEnumName = pEnumDefInfo->pszName;
			if (pEnumValInfo)
				szValueName = pEnumValInfo->pszName;
#endif

			if (szValueName)
			{
				swprintf_s(pszBuffer, cchBufferMax, L"%s", szValueName);
				return EParseResult::Success;
			}

			return EParseResult::Fail;
		}

		case Restyle::TMT_STRING:
		{
			// Documentation properties use resources for strings
			if (lpRecord->uResID)
			{
				LoadStringW(g_hThemeModule, lpRecord->uResID, pszBuffer, cchBufferMax);
			}
			else
			{
				LPCWSTR szValue = (LPCWSTR)((BYTE *)lpRecord + sizeof(VSRECORD));
				swprintf_s(pszBuffer, cchBufferMax, L"%s", szValue);
			}
			return EParseResult::Success;
		}

		case Restyle::TMT_RECT:
		{
			if (lpRecord->cbData != sizeof(RECT))
				return EParseResult::Fail;

			RECT *lpRect = (RECT *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(
				pszBuffer, cchBufferMax, L"%d, %d, %d, %d",
				lpRect->left,
				lpRect->top,
				lpRect->right,
				lpRect->bottom
			);
			return EParseResult::Success;
		}

		case Restyle::TMT_MARGINS:
		{
			if (lpRecord->cbData != sizeof(MARGINS))
				return EParseResult::Fail;

			MARGINS *lpMargins = (MARGINS *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(
				pszBuffer, cchBufferMax, L"%d, %d, %d, %d",
				lpMargins->cxLeftWidth,
				lpMargins->cxRightWidth,
				lpMargins->cyTopHeight,
				lpMargins->cyBottomHeight
			);
			return EParseResult::Success;
		}

		// Not used prior to V4 as far as I can tell.
		// But it existed and had syntax in V3, so this copies that,
		// as do the other types.
		case Restyle::TMT_INTLIST:
		{
			if (lpRecord->cbData < sizeof(int))
				return EParseResult::Fail;

			INTLIST *lpIntList = (INTLIST *)((BYTE *)lpRecord + sizeof(VSRECORD));
			// Revalidate:
			if (lpRecord->cbData < sizeof(int) + sizeof(int) * lpIntList->iValueCount)
				return EParseResult::Fail;
			
			std::wstring text;
			for (int i = 0; i < lpIntList->iValueCount; i++)
			{
				text += lpIntList->iValues[i];
				if (i != lpIntList->iValueCount - 1)
					text += L", ";
			}
			swprintf_s(pszBuffer, cchBufferMax, L"%s", text.c_str());
			return EParseResult::Success;
		}

		case Restyle::TMT_FLOATLIST:
		{
			if (lpRecord->cbData < sizeof(float))
				return EParseResult::Fail;

			FLOATLIST *lpIntList = (FLOATLIST *)((BYTE *)lpRecord + sizeof(VSRECORD));
			// Revalidate:
			if (lpRecord->cbData < sizeof(int) + sizeof(float) * lpIntList->iValueCount)
				return EParseResult::Fail;

			std::wstring text;
			for (int i = 0; i < lpIntList->iValueCount; i++)
			{
				text += lpIntList->flValues[i];
				if (i != lpIntList->iValueCount - 1)
					text += L", ";
			}
			swprintf_s(pszBuffer, cchBufferMax, L"%s", text.c_str());
			return EParseResult::Success;
		}

		case Restyle::TMT_POSITION:
		{
			if (lpRecord->cbData != sizeof(POINT))
				return EParseResult::Fail;

			POINT *lpPoint = (POINT *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"%d %d", lpPoint->x, lpPoint->y);
			return EParseResult::Success;
		}

		case Restyle::TMT_COLOR:
		{
			if (lpRecord->cbData != sizeof(COLORREF))
				return EParseResult::Fail;

			COLORREF crColor = *(COLORREF *)((BYTE *)lpRecord + sizeof(VSRECORD));
			BYTE bRed = GetRValue(crColor);
			BYTE bGreen = GetGValue(crColor);
			BYTE bBlue = GetBValue(crColor);
			swprintf_s(pszBuffer, cchBufferMax, L"%d %d %d", bRed, bGreen, bBlue);
			return EParseResult::Success;
		}

		// The data on fonts and filenames is worthless afaik. The header member
		// uResID contains the resource ID of the string these use.
		case Restyle::TMT_FONT:
		{
			// Contradictory to my statement above, UXTheme
			// fails if data size is less than 0x5C.
			if (!lpRecord->uResID || lpRecord->cbData < 0x5C)
				return EParseResult::Fail;

			LoadStringW(g_hThemeModule, lpRecord->uResID, pszBuffer, cchBufferMax);
			return EParseResult::Success;
		}

		// FileName = IMAGE resource
		// DiskStream = STREAM resource
		case Restyle::TMT_FILENAME:
		case Restyle::TMT_DISKSTREAM:
		{
			if (!lpRecord->uResID)
				return EParseResult::Fail;

			// You can't really print an image into a Win32 console window.
			// There will need to be a special handler for this when decompiling
			// themes!
			wcscpy_s(pszBuffer, cchBufferMax, L"<binary data>");
			return EParseResult::Success;
		}

		// Give raw data on unknown type for debugging
		default:
		{
			BYTE *lpData = (BYTE *)lpRecord + sizeof(VSRECORD);
			std::wstring text;
			WCHAR szTemp[3];
			for (int i = 0; i < lpRecord->cbData; i++)
			{
				swprintf_s(szTemp, L"%02X", lpData[i]);
				text += szTemp;
				if (i != (lpRecord->cbData - 1))
					text += L" ";
			}
			swprintf_s(pszBuffer, cchBufferMax, L"%s", text.c_str());
			return EParseResult::UnknownType;
		}
	}

	return EParseResult::Fail;
}

} // namespace BinParser